// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerAction.h"
#include "PlayerCharacter.generated.h"

class UAnimMontage;
class ABaseMeleeWeapon;
class UStatsComponent;
class ULookTargetComponent;

UCLASS()
class DSROGUELIKE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	APlayerCharacter();

public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable)
	void CurrentActionEnded();

	UFUNCTION(BlueprintCallable)
	void StopCurrentAction(float BlendOutTime = 0.5f);

	UFUNCTION(BlueprintCallable)
	void ExecuteAction(EActionType ActionType);

	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	virtual void BeginPlay() override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	bool IsBackstabAvailable(const FVector& Location, const FVector& Direction);

	UFUNCTION(BlueprintCallable)
	bool IsHitFromBack(const FVector& Location, const FVector& Direction);

	UFUNCTION(BlueprintCallable)
	bool IsHitBlocked(const FVector& Direction);
public:
	UPROPERTY(BlueprintReadWrite)
	ULookTargetComponent* Target;

	UPROPERTY(BlueprintReadWrite)
	ABaseMeleeWeapon *Weapon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Stats)
	float StaminaToAttack = 23.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Stats)
	float StaminaToRoll = 19.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Stats)
	float StaminaToRun = 15.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Stats)
	float StaminaToJump = 5.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Weapon)
	TSubclassOf<ABaseMeleeWeapon> WeaponClass;	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Weapon)
	FName WeaponSocket = TEXT("RightWeapon");

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Target)
	float LookToTargetPitchOffset = 15.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Target)
	float TargetBreakDistance = 1000.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Target)
	float RotateToTargetSpeed = 10.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Target)
	float RotateCameraToTargetSpeed = 10.0f;

	UPROPERTY(BlueprintReadOnly)
	UAnimMontage* MontageToPlay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* KickAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* RollAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	int32 AttackSectionNum = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* StunAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* JumpAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* AttackAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* AttackOnRunAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* BounceAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* UseAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* InteractAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* BackstabAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* BackstabAttackAnimMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Action)
	float AttackOnRunSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Action)
	float ActionMemoryTime = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Action)
	float MinRunKeyHoldTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float MovementScaleInterpSpeed = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float RunMovementScale = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float MinMovementScaleForJump = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float DefaultMovementScale = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float BlockMovementScale = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Backstab)
	float BackstabMinDot = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Backstab)
	float BackstabMaxVelocity = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Backstab)
	float BackstabDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Crit)
	float CriticalDamage = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Block)
	float BlockMinDot = -0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Kick)
	float TimeToKick = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Kick)
	float KickDistance = 70.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Kick)
	float KickDamage = 10.0f;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TEnumAsByte<ETraceTypeQuery> BackstabTraceType;
protected:
	//Weapon
	UFUNCTION(BlueprintCallable)
	void StartAttack();

	UFUNCTION(BlueprintCallable)
	void StopAttack();

	void CreateWeapon();
	//Input
	void ApplyMovementInput();

	void MoveForward(float Value);
	void MoveRight(float Value);

	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	UFUNCTION(BlueprintCallable)
	bool ContinueCurrentAction(FName& NewSection, FName& NextSection);

	UFUNCTION(BlueprintCallable)
	void Run();
	UFUNCTION(BlueprintCallable)
	void StopRunning();

	UFUNCTION(BlueprintCallable)
	void Attack();

	UFUNCTION(BlueprintCallable)
	void Block();
	UFUNCTION(BlueprintCallable)
	void StopBlocking();

	UFUNCTION(BlueprintCallable)
	void Use();
	void Interact();

	void Backstab();
	APlayerCharacter* TryToBackstab(FHitResult& HitResult);

	//Stun lock
	void StunLock();

	bool TryToKick();
	void Kick();

	//Actions
	void SetCurrentAction(EActionType ActionType);
	bool TryToSetMontage(UAnimMontage* NewMontage);

	void SetMovementScale(float NewMovementScale);
	void RotateCharaterToMovement();
	void LookToTarget();
	void ToggleTarget();

	bool TryUseStamina(float StaminaNeeded);
	bool CanAttack();

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	template<typename TEnum>
	static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value)
	{
		const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
		if (!enumPtr)
		{
			return FString("Invalid");
		}
		return enumPtr->GetNameByValue((int64)Value).ToString();
	}

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Target, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* TargetDetector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	UStatsComponent* StatsComponent;

	UPROPERTY(BlueprintReadOnly)
	FPlayerAction CurrentAction;

	UPROPERTY(BlueprintReadOnly)
	float MovementScale = 1.0f;
private:
	UPROPERTY(Transient)
	float LastIdleTime = 0.0f;

	UPROPERTY(Transient)
	int32 CurrentAttackSection = 0;

	UPROPERTY(Transient)
	FVector2D InputVector;	

	UPROPERTY(Transient)
	float TargetMovementScale = 1.0f;

	UPROPERTY(Transient)
	bool bPressedRun = false;

	UPROPERTY(Transient)
	float RunKeyHoldTime = 0.0;

	UPROPERTY(Transient)
	TArray<FPlayerAction> ActionsMemory;	
};
