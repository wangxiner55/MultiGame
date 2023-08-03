// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Data/Enum/GameEnum.h"
#include "Camera/CameraComponent.h"
#include "../Debug/DebugUtility.h"
#include "Net/UnrealNetwork.h"
#include "../Weapon/PlayerWeapon.h"
#include "../BlasterComponents/PlayerCombatComponent.h"
#include "../Interfaces/WeaponInterface.h"

// Sets default values
APlayerCharacterBase::APlayerCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetRelativeLocation((FVector(0., 0., 100)));


	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UPlayerCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);


	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	WeaponInterface = OverlapWeapon;
}



// Called when the game starts or when spawned
void APlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}


void APlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


// Called to bind functionality to input
void APlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacterBase::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacterBase::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacterBase::LookUp);

	PlayerInputComponent->BindAction("Equip", EInputEvent::IE_Pressed, this, &APlayerCharacterBase::OnEquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &APlayerCharacterBase::OnCrouchButtonPressed);
	PlayerInputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &APlayerCharacterBase::OnAimButtonPressed);
	PlayerInputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &APlayerCharacterBase::OnAimButtonReleased);
}

void APlayerCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APlayerCharacterBase, OverlapWeapon, COND_OwnerOnly);

}


//MoveComponent
void APlayerCharacterBase::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}


}

void APlayerCharacterBase::MoveRight(float Value)
{

	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacterBase::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void APlayerCharacterBase::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void APlayerCharacterBase::OnEquipButtonPressed()
{
	if (Combat)
	{
		if(HasAuthority())
		{
			Combat->EquipWeapon(OverlapWeapon);
		}
		else
		{
			ServerOnEquipButtonPressed();
		}
	}
}

void APlayerCharacterBase::OnCrouchButtonPressed()
{
	if (bIsCrouched)
	{
		UnCrouch();
		Stance = EStance::Standing;
	}
	else
	{
		Crouch();
		Stance = EStance::Crouching;
	}
}

void APlayerCharacterBase::OnAimButtonPressed()
{
	if (Combat) Combat->bisAiming = true;
}

void APlayerCharacterBase::OnAimButtonReleased()
{
	if (Combat) Combat->bisAiming = false;
}

void APlayerCharacterBase::ServerOnEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlapWeapon);
	}
}

void APlayerCharacterBase::OnRep_OverlappingWeapon(APlayerWeapon* LastWeapon)
{

	if (OverlapWeapon)
	{
		OverlapWeapon->ShowPickupWidget(true);
		
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}



void APlayerCharacterBase::SetOverlappingWeapon(APlayerWeapon* PlayerWeapon)
{
	if (OverlapWeapon) OverlapWeapon->ShowPickupWidget(false);
	OverlapWeapon = PlayerWeapon;
	if (IsLocallyControlled() && OverlapWeapon)
	{
		OverlapWeapon->ShowPickupWidget(true);
	}
}

bool APlayerCharacterBase::IsAimingState()
{
	return (Combat && Combat->bisAiming);
}

void APlayerCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}
}




void APlayerCharacterBase::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PrevCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PrevCustomMode);
	/*const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EMovementMode"), true);
	UE_LOG(LogTemp, Log, TEXT("Previous movement mode is %s"), *EnumPtr->GetNameStringByValue((int64)PrevMovementMode));

	const UEnum* EnumPtrZwa = FindObject<UEnum>(ANY_PACKAGE, TEXT("EMovementMode"), true);
	UE_LOG(LogTemp, Log, TEXT("Current movement mode is %s"), *EnumPtrZwa->GetNameStringByValue((int64)GetCharacterMovement()->MovementMode.GetValue()));*/
	EMovementMode CurrentMovementMode =  GetCharacterMovement()->MovementMode.GetValue();

	switch (CurrentMovementMode)
	{
	case MOVE_None:
		break;
	case MOVE_Walking: SetMovementState(EMovementState::Grounded);
		break;
	case MOVE_NavWalking: SetMovementState(EMovementState::Grounded);
		break;
	case MOVE_Falling: SetMovementState(EMovementState::InAir);
		break;
	case MOVE_Swimming:
		break;
	case MOVE_Flying:
		break;
	case MOVE_Custom:
		break;
	case MOVE_MAX:
		break;
	default:
		break;
	}

}


// Character Info Interface ---Start
// SetChracterInfo
FCharacterData APlayerCharacterBase::GetCharacterInfo()
{
	FCharacterData data;

	data.Velocity = GetVelocity();
	data.AimingRotation = GetControlRotation();
	data.MovementInput = GetCharacterMovement()->GetCurrentAcceleration();

	data.bHasMovementInput = false;
	data.bIsMoving = false;
	data.bIsEquipedWeapon = GetCharacterEquipState();
	data.bIsAiming = IsAimingState();

	data.Acceleration = FVector(0.0f);
	data.AimYawRate = 0.0f;
	data.Speed = 0.0f;
	data.MovementInputAmount = 0.0f;

	return data;
}

FCharacterState APlayerCharacterBase::GetCharacterState()
{
	FCharacterState state;
	state.MovementState = this->MovementState;
	state.Stance = Stance;
	return state;
}

bool APlayerCharacterBase::GetCharacterEquipState()
{
	return (Combat && Combat->EquippedWeapon);
}

void APlayerCharacterBase::SetMovementState(const enum EMovementState& state)
{
	PreMovementState = this->MovementState;
	this->MovementState = state;
}

void APlayerCharacterBase::SetStanceState(const EStance& state)
{
	this->Stance = state;
}

void APlayerCharacterBase::SetGaitState(const EGait& state)
{
	this->Gait = state;
}

void APlayerCharacterBase::SetOverlayState(const EOverlayState& state)
{
	this->OverlayState = state;
}
