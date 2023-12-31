// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Interfaces/CharacterInfoInterface.h"
#include "Components/WidgetComponent.h"
#include "PlayerCharacterBase.generated.h"

UCLASS()
class BLASTER_API APlayerCharacterBase : public ACharacter , public ICharacterInfoInterface
{
	GENERATED_BODY()

public:
	

	APlayerCharacterBase();


	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PrevCustomMode) override;
	virtual void PostInitializeComponents() override;
	

public:

	// Character Info Interface ---- Start
	virtual void SetMovementState(const enum EMovementState& state) override;
	virtual void SetGaitState(const enum EGait& state) override;
	virtual void SetStanceState(const enum EStance& state) override;
	virtual void SetOverlayState(const enum EOverlayState& state) override;
	virtual void SetRotationMode(const enum ERotationMode& state) override;

	virtual FCharacterData GetCharacterInfo() override;
	virtual FCharacterState GetCharacterState() override;
	virtual bool IsCharacterWeaponEquiped() override;
	

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void InitCharacterBaseInfo();

	UFUNCTION()
	void UpdateCharacterBaseInfo();

	UFUNCTION()
	void CharacterBaseInfo();

	UFUNCTION()
	void SaveCharacterAndControllerBaseInfo();






	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void OnEquipButtonPressed();
	void OnCrouchButtonPressed();
	void OnAimButtonPressed();
	void OnAimButtonReleased();


private:
	UPROPERTY(VisibleAnywhere, Category = CameraComponent)
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = CameraComponent)
		class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
		class APlayerWeapon* OverlapWeapon;
	

		class IWeaponInterface* WeaponInterface;

	UFUNCTION()
		void OnRep_OverlappingWeapon(APlayerWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere)
		class UPlayerCombatComponent* Combat;

	UFUNCTION(Server, Reliable)
		void ServerOnEquipButtonPressed();


	

public:

	void SetOverlappingWeapon(APlayerWeapon* PlayerWeapon);
	bool IsAimingState();
	FVector GetAcceleration();

private:

	//init
	void InitState();
	void InitControllerData();
	void ControllerBaseCoreData();
	void UpdateControllerBaseInfo();
	//Ground Rotation
	void UpdateCharacterRotator();
	void SmoothRotator(FRotator TargetRotator, float TargetInterpSpeed, float ActorInterpSpeed);
	bool CanUpdateMovingRotation();



private:
	EMovementState MovementState;
	EMovementState PreMovementState;
	EGait Gait;
	EStance Stance;
	EOverlayState OverlayState;
	ERotationMode RotationMode;

private:

	float		DeltaT;
	float		MovementInputAmount;
	bool		bIsMoving;
	bool		bHasMovementInput;
	FRotator	TargetRotation;
	FRotator	LastVelocityRotation;
	FRotator	LastMovementRotation;
	

	FRotator	AimRotation;
	float		AimYawRate;

	float		CurAimYaw;
	float		CurCharacterSpeed;
	FVector		CurCharacterAcceleration;
	FVector		CurCharacterVelocity;
	FVector		CurCharacterLocation;
	FRotator	CurCharacterRotator;

	float		PreSpeed;
	float		PreAimYaw;
	FVector		PreAcceleration;
	FVector		PreVelocity;
	FVector		PreLocation;
	FRotator	PreCharacterRotator;


	UAnimInstance*					AnimInstance;
	UCharacterMovementComponent*	CharacterMovement;
};

USTRUCT()
struct FCharacterCoreBaseData
{
	GENERATED_BODY()

public:

	// Rotator
	FRotator	CharacterRotator;
	float		AimYaw;

	// Movement
	FVector		Acceleration;
	FVector		Velocity;
	FVector		Location;
	float		Speed;


};


USTRUCT()
struct FControllerCoreBaseData
{
	GENERATED_BODY()

public:

	// Rotator
	FRotator	AimRotator;
	float		AimYaw;
	float		AimPitch;

	// Movement
	float		ControllerMovementInput;
	bool		bHasControllerMovementInput;



};