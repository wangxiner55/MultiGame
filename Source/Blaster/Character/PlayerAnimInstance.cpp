// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "../Debug/DebugUtility.h"
#include "PlayerCharacterBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Actor.h"

void UPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	
	
	
	LeanAmount = FVector2D(0.0f);
	LeanInterpSpeed = 4.0f;
	
	
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Owner = TryGetPawnOwner();
	if (Owner == nullptr) return;
	Character = Cast<ACharacter>(Owner);
	playerInterface = Cast<ICharacterInfoInterface>(Character);

	//考虑从角色传入，这个在角色部分已经实现过
	CharacterMovement = Character->GetCharacterMovement();
	ControlRotation = Character->GetControlRotation();
	
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	DeltaT = DeltaSeconds;

	// Check  Character ptr
	if(Character == nullptr)
	Character = Cast<ACharacter>(TryGetPawnOwner());
	if (Character == nullptr) return;


	UpdatePlayerBaseInfo();
	
	if (MoveCheck())
	{
		UpdateMovementValue();
	}
	//

}

void UPlayerAnimInstance::UpdatePlayerBaseInfo()
{
	GetPlayerCharacterInfo();
	GetPlayerCharacterState();
	SetAnimPlayerBaseState();
	SetAnimPlayerBaseValue();
	

}

void UPlayerAnimInstance::GetPlayerCharacterInfo()
{

	if(playerInterface)
	CharacterData = playerInterface->GetCharacterInfo();

}

void UPlayerAnimInstance::GetPlayerCharacterState()
{
	if (playerInterface)
	CharacterState = playerInterface->GetCharacterState();
}

void UPlayerAnimInstance::SetAnimPlayerBaseValue()
{
	Velocity				=		CharacterData.Velocity;
	Speed					=		CharacterData.Speed;
	Acceleration			=		CharacterData.Acceleration;
	MovementInput			=		CharacterData.MovementInput;
	IsMoving				=		CharacterData.bIsMoving;
	IsAiming				=		CharacterData.bIsAiming;
	HasMovementInput		=		CharacterData.bHasMovementInput;
	MovementInputAmount		=		CharacterData.MovementInputAmount;
	AimYawRate				=		CharacterData.AimYawRate;
	AimingRotation			=		CharacterData.AimingRotation;
	bIsEquipWeapon			=		CharacterData.bIsEquipedWeapon;

}

void UPlayerAnimInstance::SetAnimPlayerBaseState()
{
	MovementState			 =			CharacterState.MovementState;
	Stance					 =			CharacterState.Stance;
	OverlayState			 =			CharacterState.OverlayState;
	Gait					 =			CharacterState.Gait;
	RotationMode			 =			CharacterState.RotationMode;
}

bool UPlayerAnimInstance::MoveCheck()
{
	return IsMoving && HasMovementInput || Speed > 150.0f;
}

void UPlayerAnimInstance::UpdateMovementValue()
{
	FVector AccAmount = CalculateRelativeAccelerationAmount();
	FVector2D TargetLeanAmount = FVector2D(AccAmount.Y, AccAmount.X);
	LeanAmount = InterpLeanAmount(LeanAmount, TargetLeanAmount, LeanInterpSpeed, DeltaT);
}

FVector UPlayerAnimInstance::CalculateRelativeAccelerationAmount()
{

	bool bIsSameless = Acceleration.Dot(Velocity) > 0.0f ? true : false;
	if (bIsSameless)
	{
		float MaxAcc = CharacterMovement->GetMaxAcceleration();
		return UKismetMathLibrary::LessLess_VectorRotator(
			UKismetMathLibrary::Vector_ClampSizeMax(Acceleration, MaxAcc) / MaxAcc,
			Character->GetActorRotation());
	}
	else
	{
		float MaxAcc = CharacterMovement->GetMaxBrakingDeceleration();
		return UKismetMathLibrary::LessLess_VectorRotator(
			UKismetMathLibrary::Vector_ClampSizeMax(Acceleration, MaxAcc) / MaxAcc,
			Character->GetActorRotation());
	}
}

FVector2D UPlayerAnimInstance::InterpLeanAmount(FVector2D Current, FVector2D Target, float InterpSpeed, float DeltaTime)
{
	return FVector2D(
		UKismetMathLibrary::FInterpTo(Current.X, Target.X, DeltaTime, InterpSpeed),
		UKismetMathLibrary::FInterpTo(Current.Y, Target.Y, DeltaTime, InterpSpeed));
}

void UPlayerAnimInstance::SetVelocityBlend()
{
	FVector4 TargetVelocityBlend = CalcVelocityBlend();
	

	VelocityBlend = FVector4(
		UKismetMathLibrary::FInterpTo(VelocityBlend.X, TargetVelocityBlend.X, DeltaT, VelocityBlendInterpSpeed),
		UKismetMathLibrary::FInterpTo(VelocityBlend.Y, TargetVelocityBlend.Y, DeltaT, VelocityBlendInterpSpeed),
		UKismetMathLibrary::FInterpTo(VelocityBlend.Z, TargetVelocityBlend.Z, DeltaT, VelocityBlendInterpSpeed),
		UKismetMathLibrary::FInterpTo(VelocityBlend.W, TargetVelocityBlend.W, DeltaT, VelocityBlendInterpSpeed));
}


FVector4 UPlayerAnimInstance::CalcVelocityBlend()
{
	FVector RelativeVelocity = Character->GetActorRotation().UnrotateVector(Velocity.GetSafeNormal());
	FVector RelVel = RelativeVelocity / (RelativeVelocity.X + RelativeVelocity.Y + RelativeVelocity.Z);
	return FVector4(RelVel.X, abs((RelVel.X)), RelVel.Y, abs((RelVel.Y)));
}
