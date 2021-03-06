﻿#include "Player/PlayerCharacter.h"

#include "Animation/AnimMontage.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/Components/LookTargetComponent.h"
#include "Player/Components/StatsComponent.h"
#include "Weapon/BaseMeleeWeapon.h"
#include "Weapon/DamageType/MeleeDamageEvent.h"

//////////////////////////////////////////////////////////////////////////
// ATempCharacter

APlayerCharacter::APlayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;				// Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);	// ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;		   // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true;	   // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(
		CameraBoom, USpringArmComponent::SocketName);	 // Attach the camera to the end of the boom and let the boom adjust to
														 // match the controller orientation
	FollowCamera->bUsePawnControlRotation = false;		 // Camera does not rotate relative to arm

	StatsComponent = CreateDefaultSubobject<UStatsComponent>(TEXT("StatsComponent"));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	TargetDetector = CreateDefaultSubobject<USphereComponent>(TEXT("TargetDetector"));
	TargetDetector->SetupAttachment(RootComponent);
	TargetDetector->SetSphereRadius(1000.0f);
	TargetDetector->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TargetDetector->SetCollisionResponseToAllChannels(ECR_Ignore);
	TargetDetector->SetCollisionObjectType(ECC_WorldDynamic);
	TargetDetector->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	EstusFlaskMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EstusFlaskMesh"));
	EstusFlaskMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EstusFlaskMesh->SetupAttachment(GetMesh(), TEXT("SpineSocket"));
}

void APlayerCharacter::CurrentActionEnded()
{
	FName NewSection;
	FName NextSection;

	if (MontageToPlay && ContinueCurrentAction(NewSection, NextSection))
	{
		if (auto AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_SetNextSection(NewSection, NextSection, MontageToPlay);
		}
	}
	else
	{
		StopCurrentAction();
	}
}

void APlayerCharacter::StopCurrentAction(float BlendOutTime)
{
	switch (CurrentAction.ActionType)
	{
		case EActionType::AT_Roll:
			SetMovementScale(DefaultMovementScale);
		case EActionType::AT_Attack:
		case EActionType::AT_AttackOnRun:
			StopAttack();
			break;
		default:
			break;
	}

	EstusFlaskMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("SpineSocket"));

	TargetMovementScale = DefaultMovementScale;
	if (MontageToPlay)
	{
		if (auto AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Stop(BlendOutTime, MontageToPlay);
		}
	}
	MontageToPlay = nullptr;
	CurrentAction.ActionType = EActionType::AT_Idle;
}

void APlayerCharacter::ExecuteAction(EActionType ActionType)
{
	if (CurrentAction.ActionType == EActionType::AT_Idle)
	{
		SetCurrentAction(ActionType);
	}
	else
	{
		ActionsMemory.Insert(FPlayerAction(GetWorld()->TimeSeconds, ActionType), 0);
	}
}

void APlayerCharacter::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	float TimeToCheck = GetWorld()->TimeSeconds - ActionMemoryTime;
	for (int32 I = ActionsMemory.Num() - 1; I >= 0; --I)
	{
		if (ActionsMemory[I].ExecuteTime < TimeToCheck)
		{
			ActionsMemory.RemoveAt(I);
		}
		else
		{
			break;
		}
	}

	if (CurrentAction.ActionType == EActionType::AT_Idle && ActionsMemory.Num() > 0)
	{
		SetCurrentAction(ActionsMemory[0].ActionType);
		ActionsMemory.RemoveAt(0);
	}

	if (bPressedRun)
	{
		RunKeyHoldTime += DeltaTime;
		if (RunKeyHoldTime > MinRunKeyHoldTime)
		{
			if (CurrentAction.ActionType == EActionType::AT_Idle)
			{
				ExecuteAction(EActionType::AT_Run);
			}
			bPressedRun = false;
		}
	}

	if (CurrentAction.ActionType == EActionType::AT_Run)
	{
		if (!TryUseStamina(StaminaToRun * DeltaTime))
		{
			StopCurrentAction();
		}
	}

	MovementScale = FMath::FInterpTo(MovementScale, TargetMovementScale, DeltaTime, MovementScaleInterpSpeed);
	ApplyMovementInput();
	LookToTarget();
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetMovementScale(DefaultMovementScale);
	CreateWeapon();
}

float APlayerCharacter::TakeDamage(
	float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage <= 0.0f)
	{
		return ActualDamage;
	}

	if (DamageEvent.IsOfType(FMeleeDamageEvent::ClassID))
	{
		// point damage event, pass off to helper function
		FMeleeDamageEvent* const MeleeDamageEvent = (FMeleeDamageEvent*) &DamageEvent;
		if (CurrentAction.ActionType == EActionType::AT_Block)
		{
			if (IsHitBlocked(MeleeDamageEvent->ShotDirection))
			{
				StatsComponent->ApplyDamage(EStatsParameterType::SP_Stamina, MeleeDamageEvent->StaminaDamage);
				FStatsParameter Stamina;
				StatsComponent->GetStatsParameter(EStatsParameterType::SP_Stamina, Stamina);
				if (Stamina.Value < 0.0f)
				{
					StunLock();
				}
				return 0.0f;
			}
		}

		ActualDamage = MeleeDamageEvent->Damage;
		StatsComponent->ApplyDamage(EStatsParameterType::SP_Health, MeleeDamageEvent->Damage);
		StatsComponent->ApplyDamage(EStatsParameterType::SP_Poise, MeleeDamageEvent->PoiseDamage);
		FStatsParameter Poise;
		StatsComponent->GetStatsParameter(EStatsParameterType::SP_Poise, Poise);
		if (Poise.Value <= 0.0f)
		{
			Stagger();
		}
	}

	return ActualDamage;
}

bool APlayerCharacter::IsBackstabAvailable(const FVector& Location, const FVector& Direction)
{
	if (GetVelocity().SizeSquared() > (BackstabMaxVelocity * BackstabMaxVelocity))
	{
		UE_LOG(LogTemp, Log, TEXT("Enemy velocity too high"));
		return false;
	}

	switch (CurrentAction.ActionType)
	{
		case EActionType::AT_Backstab:
		case EActionType::AT_BackstabAttack:
		case EActionType::AT_Bounce:
		case EActionType::AT_Jump:
		case EActionType::AT_Run:
			UE_LOG(LogTemp, Log, TEXT("Wrong action"));
			return false;
		default:
			break;
	}
	const FVector& Forward = GetActorForwardVector();
	return IsHitFromDirection(Location, Direction, Forward);
}

bool APlayerCharacter::IsFrontstabAvailable(const FVector& Location, const FVector& Direction)
{
	if (CurrentAction.ActionType != EActionType::AT_Stun)
	{
		UE_LOG(LogTemp, Log, TEXT("Not stun action"));
		return false;
	}

	const FVector& Forward = GetActorForwardVector();
	return IsHitFromDirection(Location, Direction, -Forward);
}

bool APlayerCharacter::IsHitFromDirection(const FVector& Location, const FVector& HitDirection, const FVector& Direction)
{
	if (FVector::DotProduct(Direction, HitDirection) < CriticalMinDot)
	{
		UE_LOG(LogTemp, Log, TEXT("Wrong attack direction"));
		return false;
	}

	FVector AttackDirection = GetActorLocation() - Location;
	AttackDirection.Z = 0.0f;
	AttackDirection.Normalize();

	if (FVector::DotProduct(Direction, AttackDirection) < CriticalMinDot)
	{
		UE_LOG(LogTemp, Log, TEXT("Wrong attack location"));
		return false;
	}

	return true;
}

bool APlayerCharacter::IsHitBlocked(const FVector& Direction)
{
	const FVector& Forward = GetActorForwardVector();
	if (FVector::DotProduct(Forward, Direction) > BlockMinDot)
	{
		UE_LOG(LogTemp, Log, TEXT("Wrong attack direction"));
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// Input

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &APlayerCharacter::Run);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &APlayerCharacter::StopRunning);

	PlayerInputComponent->BindAction("Use", IE_Pressed, this, &APlayerCharacter::Use);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &APlayerCharacter::Attack);

	PlayerInputComponent->BindAction("Block", IE_Pressed, this, &APlayerCharacter::Block);
	PlayerInputComponent->BindAction("Block", IE_Released, this, &APlayerCharacter::StopBlocking);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::Interact);
	PlayerInputComponent->BindAction("ToggleTarget", IE_Pressed, this, &APlayerCharacter::ToggleTarget);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &APlayerCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &APlayerCharacter::TouchStopped);
}

void APlayerCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void APlayerCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

bool APlayerCharacter::ContinueCurrentAction(FName& NewSection, FName& NextSection)
{
	bool Result = false;
	EActionType NextAction = ActionsMemory.Num() > 0 ? ActionsMemory[0].ActionType : EActionType::AT_Block;

	switch (CurrentAction.ActionType)
	{
		case EActionType::AT_Attack:
		{
			if (NextAction == EActionType::AT_Attack)
			{
				if (CanAttack())
				{
					CurrentAttackSection = (CurrentAttackSection + 1) % AttackSectionNum;
					NewSection = *FString::Printf(TEXT("AttackStart%d"), CurrentAttackSection);
					NextSection = *FString::Printf(TEXT("AttackStart%d"), CurrentAttackSection + 1);
					Result = true;
				}
			}
			break;
		}
		case EActionType::AT_Block:
		{
			if (NextAction == EActionType::AT_Block)
			{
				NewSection = TEXT("Block");
				NextSection = TEXT("BlockEnd");
				Result = true;
			}
			break;
		}
	}

	return Result;
}

void APlayerCharacter::Run()
{
	if (GetCharacterMovement()->IsFalling())
	{
		return;
	}

	bPressedRun = false;
	if (MovementScale > MinMovementScaleForJump)
	{
		ExecuteAction(EActionType::AT_Jump);
		return;
	}

	const float Epsilon = 0.00001f;

	if (InputVector.SizeSquared() > Epsilon)
	{
		RunKeyHoldTime = 0.0f;
		bPressedRun = true;
	}
	else
	{
		if (MovementScale < DefaultMovementScale + Epsilon)
		{
			ExecuteAction(EActionType::AT_Bounce);
		}
	}
}

void APlayerCharacter::StopRunning()
{
	if (bPressedRun && RunKeyHoldTime < MinRunKeyHoldTime)
	{
		ExecuteAction(EActionType::AT_Roll);
	}
	else
	{
		if (CurrentAction.ActionType == EActionType::AT_Run)
		{
			StopCurrentAction();
		}
	}

	bPressedRun = false;
}

void APlayerCharacter::Attack()
{
	if (GetCharacterMovement()->IsFalling())
	{
		// Falling attack
		return;
	}

	FHitResult HitResult;
	bool bIsBackStatab;
	if (auto Enemy = TryToCriticalAttack(HitResult, bIsBackStatab))
	{
		Enemy->CriticalAttack(bIsBackStatab);
		Enemy->TakeDamage(KickDamage,
			FMeleeDamageEvent(CriticalDamage, 0.0f, 0.0f, HitResult, GetActorForwardVector(), UDamageType::StaticClass()), nullptr,
			this);
		EActionType Action = bIsBackStatab ? EActionType::AT_BackstabAttack : EActionType::AT_FrontstabAttack;
		ExecuteAction(Action);
	}
	else
	{
		if (CurrentAction.ActionType == EActionType::AT_Run)
		{
			StopCurrentAction();
		}

		if (GetVelocity().Size() > AttackOnRunSpeed)
		{
			ExecuteAction(EActionType::AT_AttackOnRun);
		}
		else
		{
			if (!TryToKick())
			{
				ExecuteAction(EActionType::AT_Attack);
			}
		}
	}
}

void APlayerCharacter::Block()
{
	ExecuteAction(EActionType::AT_Block);
}

void APlayerCharacter::StopBlocking()
{
	if (CurrentAction.ActionType == EActionType::AT_Block)
	{
		StopCurrentAction();
	}
}

void APlayerCharacter::Use()
{
	ExecuteAction(EActionType::AT_Use);
}

void APlayerCharacter::Interact()
{
	ExecuteAction(EActionType::AT_Interact);
}

void APlayerCharacter::CriticalAttack(bool IsBackstab)
{
	EActionType ActionType = IsBackstab ? EActionType::AT_Backstab : EActionType::AT_Frontstab;
	ExecuteAction(ActionType);
	if (CurrentAction.ActionType != ActionType)
	{
		StopCurrentAction(0.0f);
	}
}

APlayerCharacter* APlayerCharacter::TryToCriticalAttack(FHitResult& HitResult, bool& IsBackstab)
{
	if (CurrentAction.ActionType != EActionType::AT_Idle)
	{
		return nullptr;
	}

	if (InputVector.SizeSquared() > 0.0001f)
	{
		return nullptr;
	}

	if (GetVelocity().Size() > BackstabMaxVelocity)
	{
		return nullptr;
	}

	const FVector EndLocation = GetActorLocation() + GetActorForwardVector() * CriticalDistance;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Push(this);
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), GetActorLocation(), EndLocation, BackstabTraceType, false, ActorsToIgnore,
			EDrawDebugTrace::None, HitResult, true, FLinearColor::Blue))
	{
		auto Enemy = Cast<APlayerCharacter>(HitResult.GetActor());
		if (Enemy)
		{
			const FVector& Forward = GetActorForwardVector();
			if (Enemy->IsBackstabAvailable(HitResult.ImpactPoint, Forward))
			{
				IsBackstab = true;
				return Enemy;
			}
			if (Enemy->IsFrontstabAvailable(HitResult.ImpactPoint, Forward))
			{
				IsBackstab = false;
				return Enemy;
			}
		}
	}
	return nullptr;
}

void APlayerCharacter::StunLock()
{
	StopCurrentAction();
	ExecuteAction(EActionType::AT_Stun);
}

bool APlayerCharacter::TryToKick()
{
	if (InputVector.X > 0.9f && InputVector.Y == 0.0f)
	{
		if (GetWorld()->TimeSeconds - LastIdleTime < TimeToKick)
		{
			ExecuteAction(EActionType::AT_Kick);
			return true;
		}
	}
	return false;
}

void APlayerCharacter::Kick()
{
	const FVector& HitDirection = GetActorForwardVector();
	const FVector EndLocation = GetActorLocation() + HitDirection * KickDistance;
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Push(this);
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), GetActorLocation(), EndLocation, BackstabTraceType, false, ActorsToIgnore,
			EDrawDebugTrace::None, HitResult, true, FLinearColor::Blue))
	{
		auto Enemy = HitResult.GetActor();
		if (Enemy)
		{
			Enemy->TakeDamage(KickDamage,
				FMeleeDamageEvent(KickDamage, 101.0f, 0.0f, HitResult, HitDirection, UDamageType::StaticClass()), nullptr, this);
		}
	}
}

void APlayerCharacter::Stagger()
{
	switch (CurrentAction.ActionType)
	{
		case EActionType::AT_Frontstab:
		case EActionType::AT_Backstab:
		case EActionType::AT_Stun:
		case EActionType::AT_Stagger:
			return;
		default:
			break;
	}

	StopCurrentAction();
	ExecuteAction(EActionType::AT_Stagger);
}

void APlayerCharacter::SetCurrentAction(EActionType ActionType)
{
	FString ParseLine = GetEnumValueAsString<EActionType>("EActionType", ActionType);
	UE_LOG(LogTemp, Log, TEXT("New ActionType: %s"), *ParseLine);

	StopCurrentAction();

	switch (ActionType)
	{
		case EActionType::AT_Attack:
			if (!CanAttack())
			{
				return;
			}
			CurrentAttackSection = 0;
			TryToSetMontage(AttackAnimMontage);
			break;
		case EActionType::AT_AttackOnRun:
			if (!CanAttack())
			{
				return;
			}
			TryToSetMontage(AttackOnRunAnimMontage);
			break;
		case EActionType::AT_Stagger:
			TryToSetMontage(StaggerAnimMontage);
			break;
		case EActionType::AT_Backstab:
			TryToSetMontage(BackstabAnimMontage);
			break;
		case EActionType::AT_BackstabAttack:
			TryToSetMontage(BackstabAttackAnimMontage);
			break;
		case EActionType::AT_Frontstab:
			TryToSetMontage(FrontstabAnimMontage);
			break;
		case EActionType::AT_FrontstabAttack:
			TryToSetMontage(FrontstabAttackAnimMontage);
			break;
		case EActionType::AT_Block:
			TargetMovementScale = BlockMovementScale;
			SetMovementScale(BlockMovementScale);
			break;
		case EActionType::AT_Bounce:
			TryToSetMontage(BounceAnimMontage);
			break;
		case EActionType::AT_Interact:
			TryToSetMontage(InteractAnimMontage);
			break;
		case EActionType::AT_Jump:
			if (!TryUseStamina(StaminaToJump))
			{
				return;
			}
			TryToSetMontage(JumpAnimMontage);
			break;
		case EActionType::AT_Kick:
			TryToSetMontage(KickAnimMontage);
			Kick();
			break;
		case EActionType::AT_Roll:
			if (!TryUseStamina(StaminaToRoll))
			{
				return;
			}
			RotateCharaterToMovement();
			TryToSetMontage(RollAnimMontage);
			break;
		case EActionType::AT_Run:
			if (!TryUseStamina(StaminaToRun))
			{
				return;
			}
			TargetMovementScale = RunMovementScale;
			break;
		case EActionType::AT_Stun:
			TryToSetMontage(StunAnimMontage);
			break;
		case EActionType::AT_Use:
			TryToSetMontage(UseAnimMontage);
			EstusFlaskMesh->AttachToComponent(
				GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("LeftHandSocket"));
			break;
		default:
			break;
	}

	CurrentAction.ActionType = ActionType;
}

bool APlayerCharacter::TryToSetMontage(UAnimMontage* NewMontage)
{
	if (!NewMontage)
	{
		StopCurrentAction();
		return false;
	}

	SetMovementScale(0.0f);
	MontageToPlay = NewMontage;
	if (auto AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Play(MontageToPlay);
	}
	return true;
}

void APlayerCharacter::SetMovementScale(float NewMovementScale)
{
	MovementScale = NewMovementScale;
	TargetMovementScale = NewMovementScale;
}

void APlayerCharacter::RotateCharaterToMovement()
{
	if (!Controller)
	{
		return;
	}

	if (InputVector.SizeSquared() > 0.0f)
	{
		const FRotator& Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

		// get forward vector
		const FRotationMatrix RotationMatrix(YawRotation);
		const FVector ForwardDirection = RotationMatrix.GetUnitAxis(EAxis::X) * InputVector.X;
		const FVector RightDirection = RotationMatrix.GetUnitAxis(EAxis::Y) * InputVector.Y;

		FVector Direction = ForwardDirection + RightDirection;
		Direction.Normalize();

		FRotator ActorRotation = GetActorRotation();
		ActorRotation.Yaw = FMath::RadiansToDegrees(FMath::Atan2(Direction.Y, Direction.X));
		SetActorRotation(ActorRotation);
	}
}

void APlayerCharacter::LookToTarget()
{
	if (Target)
	{
		float TargetDistanceSquared = (GetActorLocation() - Target->GetComponentLocation()).SizeSquared();
		if (TargetDistanceSquared > TargetBreakDistance * TargetBreakDistance)
		{
			ToggleTarget();
			return;
		}

		FRotator NewControlRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetComponentLocation());
		NewControlRotation.Pitch += LookToTargetPitchOffset;
		const FRotator& CurrentControllerRotation = Controller->GetControlRotation();
		const FRotator& TargetControllerRotation = FMath::RInterpTo(
			CurrentControllerRotation, NewControlRotation, GetWorld()->DeltaTimeSeconds, RotateCameraToTargetSpeed);
		Controller->SetControlRotation(TargetControllerRotation);

		if (CurrentAction.ActionType == EActionType::AT_Idle || CurrentAction.ActionType == EActionType::AT_Block)
		{
			GetCharacterMovement()->bOrientRotationToMovement = false;
			const FRotator& Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);
			const FRotator& CurrentRotation = GetActorRotation();
			const FRotator& TargetRotation =
				FMath::RInterpTo(CurrentRotation, YawRotation, GetWorld()->DeltaTimeSeconds, RotateToTargetSpeed);
			SetActorRotation(TargetRotation);
		}
		else
		{
			GetCharacterMovement()->bOrientRotationToMovement = true;
		}
	}
}

void APlayerCharacter::ToggleTarget()
{
	if (Target)
	{
		Target->DeactivateTarget();
		GetCharacterMovement()->bOrientRotationToMovement = true;
		Target = nullptr;
		return;
	}
	TArray<AActor*> OtherCharacters;
	TargetDetector->GetOverlappingActors(OtherCharacters, ACharacter::StaticClass());
	for (auto Character : OtherCharacters)
	{
		if (Character != this)
		{
			Target = Character->FindComponentByClass<ULookTargetComponent>();
			if (Target)
			{
				Target->ActivateTarget(FollowCamera);
				return;
			}
		}
	}
}

bool APlayerCharacter::TryUseStamina(float StaminaNeeded)
{
	FStatsParameter Stamina;
	StatsComponent->GetStatsParameter(EStatsParameterType::SP_Stamina, Stamina);
	if (Stamina.Value >= 0.0f)
	{
		StatsComponent->ApplyDamage(EStatsParameterType::SP_Stamina, StaminaNeeded);
		return true;
	}

	return false;
}

bool APlayerCharacter::CanAttack()
{
	UPawnMovementComponent* MovementComponent = GetMovementComponent();
	if (MovementComponent->IsFalling())
	{
		return false;
	}

	return TryUseStamina(StaminaToAttack);
}

void APlayerCharacter::TurnAtRate(float Rate)
{
	if (Target)
	{
		return;
	}
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Rate)
{
	if (Target)
	{
		return;
	}
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::StartAttack()
{
	if (Weapon)
	{
		Weapon->StartAttack();
	}
}

void APlayerCharacter::StopAttack()
{
	if (Weapon)
	{
		Weapon->StopAttack();
	}
}

void APlayerCharacter::CreateWeapon()
{
	if (Weapon)
	{
		return;
	}

	if (!WeaponClass)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Weapon = GetWorld()->SpawnActor<ABaseMeleeWeapon>(WeaponClass, Params);
	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
}

void APlayerCharacter::ApplyMovementInput()
{
	if ((Controller != NULL) && MovementScale > 0.0f)
	{
		if (InputVector.SizeSquared() > 0.0001f)
		{
			// find out which way is right
			const FRotator& Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

			// get right vector
			const FVector& DirectionY = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			const FVector& DirectionX = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			FVector Direction = DirectionX * InputVector.X + DirectionY * InputVector.Y;
			Direction.Normalize();
			// add movement in that direction
			AddMovementInput(Direction, MovementScale);
		}
		else
		{
			LastIdleTime = GetWorld()->TimeSeconds;
		}
	}
}

void APlayerCharacter::MoveForward(float Value)
{
	InputVector.X = Value;
}

void APlayerCharacter::MoveRight(float Value)
{
	InputVector.Y = Value;
}
