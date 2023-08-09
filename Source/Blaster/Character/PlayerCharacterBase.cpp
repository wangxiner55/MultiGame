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
#include "Kismet/KismetMathLibrary.h"

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
	

	CharacterMovement = GetCharacterMovement() == nullptr ? nullptr : GetCharacterMovement();
	AnimInstance = this->GetMesh()->GetAnimInstance() == nullptr ? nullptr : this->GetMesh()->GetAnimInstance();
	InitState();
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	//WeaponInterface = OverlapWeapon;
}



// Called when the game starts or when spawned
void APlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	//ensure current Data update before with animInstance and Mesh
	this->GetMesh()->AddTickPrerequisiteActor(this);


	CharacterMovement	= GetCharacterMovement()				== nullptr ? nullptr : GetCharacterMovement();
	AnimInstance		= this->GetMesh()->GetAnimInstance()	== nullptr ? nullptr : this->GetMesh()->GetAnimInstance();
	

	InitControllerData();
	InitCharacterBaseInfo();
}

void APlayerCharacterBase::InitState()
{
	RotationMode = ERotationMode::VelocityDirection;
	Gait = EGait::Walking;
	Stance = EStance::Standing;
	OverlayState = EOverlayState::Default;
	MovementState = EMovementState::Grounded;
	PreMovementState = EMovementState::Grounded;
}


void APlayerCharacterBase::InitControllerData()
{
	if (Controller == nullptr)return;
	ControllerBaseCoreData();
}



//Character Base Info
void APlayerCharacterBase::InitCharacterBaseInfo()
{
	
	
	LastMovementRotation = LastVelocityRotation = TargetRotation = GetActorRotation();

	UpdateCharacterBaseInfo();
	SaveCharacterAndControllerBaseInfo();
}

void APlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//SetTickTime
	DeltaT = DeltaTime;
	
	UpdateCharacterBaseInfo();
	UpdateControllerBaseInfo();


	SaveCharacterAndControllerBaseInfo();
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


//Replicat RPC
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
	if (Combat) Combat->SetAiming(true);
}

void APlayerCharacterBase::OnAimButtonReleased()
{
	if (Combat) Combat->SetAiming(false);
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

void APlayerCharacterBase::CharacterBaseInfo()
{
	

	CurCharacterVelocity		= GetVelocity();
	LastVelocityRotation		= UKismetMathLibrary::MakeRotFromX(CurCharacterVelocity);
	
	CurCharacterAcceleration = GetAcceleration();

	CurCharacterLocation		= GetActorLocation();

	CurCharacterRotator			= GetActorRotation(); 
								
	CurCharacterSpeed			= FVector2D(CurCharacterVelocity.X, CurCharacterVelocity.Y).Length();
	bIsMoving					= CurCharacterSpeed > 1.0 ? true : false;			
}

void APlayerCharacterBase::ControllerBaseCoreData()
{
	AimRotation						= GetBaseAimRotation();
	CurAimYaw						= AimRotation.Yaw;
	MovementInputAmount				= CharacterMovement == nullptr ? 0.0f : CharacterMovement->GetCurrentAcceleration().Length() / CharacterMovement->GetMaxAcceleration();
	AimYawRate						= (CurAimYaw - PreAimYaw) / DeltaT;
	bHasMovementInput				= MovementInputAmount > 0.0f ? true : false;
	LastMovementRotation			= CharacterMovement == nullptr ? GetActorRotation() : UKismetMathLibrary::MakeRotFromX(CharacterMovement->GetCurrentAcceleration());
}

void APlayerCharacterBase::UpdateCharacterBaseInfo()
{


	CharacterBaseInfo();
	

	////Update AimYawOffset
	//FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(CurCharacterVelocity);
	//AimYawRate = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
	//
	//FRotator DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(PreCharacterRotator, CurCharacterRotator);

}

void APlayerCharacterBase::UpdateControllerBaseInfo()
{
	ControllerBaseCoreData();
}

void APlayerCharacterBase::SaveCharacterAndControllerBaseInfo()
{
	PreAcceleration				= CurCharacterAcceleration;
	PreVelocity					= CurCharacterVelocity;
	PreLocation					= CurCharacterLocation;
	PreCharacterRotator			= CurCharacterRotator;
	PreSpeed					= CurCharacterSpeed;
	PreAimYaw					= CurAimYaw;
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

FVector APlayerCharacterBase::GetAcceleration()
{
	return (CurCharacterVelocity - PreVelocity) / DeltaT;
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

// Character Rotator Caculate

void APlayerCharacterBase::UpdateCharacterRotator()
{
	
	if (CanUpdateMovingRotation())
	{
		switch (RotationMode)
		{
		case ERotationMode::VelocityDirection:
			SmoothRotator(LastVelocityRotation, 800.0f, 10.0f);
			
			break;
		case ERotationMode::LookingDirection:
			break;
		case ERotationMode::Aiming:
			break;
		default:
			break;
		}
	}
}

void APlayerCharacterBase::SmoothRotator(FRotator TargetRotator, float TargetInterpSpeed, float ActorInterpSpeed)
{
	TargetRotation = UKismetMathLibrary::RInterpTo_Constant(TargetRotation, TargetRotator, DeltaT, TargetInterpSpeed);
}

bool APlayerCharacterBase::CanUpdateMovingRotation()
{
	return true;
}

// Character Info Interface ---Start
// SetChracterInfo
FCharacterData APlayerCharacterBase::GetCharacterInfo()
{
	FCharacterData data;

	data.Acceleration			 =				CurCharacterAcceleration;
	data.Velocity				 =				CurCharacterVelocity;
	data.AimingRotation			 =				AimRotation;
	data.MovementInput			 =				CharacterMovement->GetCurrentAcceleration();

	data.bHasMovementInput		 =				bHasMovementInput;
	data.bIsMoving				 =				bIsMoving;
	data.bIsEquipedWeapon		 =				IsCharacterWeaponEquiped();
	data.bIsAiming				 =				IsAimingState();

	data.AimYawRate				 =				AimYawRate;
	data.Speed					 =				CurCharacterSpeed;
	data.MovementInputAmount	 =				MovementInputAmount;

	return data;
}

FCharacterState APlayerCharacterBase::GetCharacterState()
{
	FCharacterState state;
	state.MovementState = MovementState;
	state.Stance = Stance;
	state.RotationMode = RotationMode;
	return state;
}

bool APlayerCharacterBase::IsCharacterWeaponEquiped()
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
	if (OverlayState != state)
	{
		OverlayState = state;

	}
}

void APlayerCharacterBase::SetRotationMode(const ERotationMode& state)
{
	if (RotationMode != state)
	{
		RotationMode = state;
		
	}

}

bool APlayerCharacterBase::IsAimingState()
{
	return (Combat && Combat->bisAiming);
}