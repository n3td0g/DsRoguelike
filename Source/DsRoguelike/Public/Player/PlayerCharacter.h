// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerAction.h"
#include "PlayerCharacter.generated.h"

class UAnimMontage;

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
	void StopCurrentAction();

	UFUNCTION(BlueprintCallable)
	void ExecuteAction(EActionType ActionType);

	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	bool IsBackstabAvailable(const FVector& Location, const FVector& Direction);

public:
	UPROPERTY(BlueprintReadWrite)
	class ULookTargetComponent* Target;

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
	UAnimMontage* RollAnimMontage;

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

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TEnumAsByte<ETraceTypeQuery> BackstabTraceType;
protected:
	void ApplyMovementInput();

	void MoveForward(float Value);
	void MoveRight(float Value);

	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	void Run();
	void StopRunning();

	void Attack();

	void Block();
	void StopBlocking();

	void Use();
	void Interact();

	void Backstab();
	APlayerCharacter* TryToBackstab();

	void SetCurrentAction(EActionType ActionType);
	bool TryToSetMontage(UAnimMontage* NewMontage);

	void SetMovementScale(float NewMovementScale);
	void RotateCharaterToMovement();
	void LookToTarget();
	void ToggleTarget();

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

	UPROPERTY(BlueprintReadOnly)
	FPlayerAction CurrentAction;

	UPROPERTY(BlueprintReadOnly)
	float MovementScale = 1.0f;
private:
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
