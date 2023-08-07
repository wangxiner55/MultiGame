// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCombatComponent.h"
#include "../Weapon/PlayerWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "../Character/PlayerCharacterBase.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UPlayerCombatComponent::UPlayerCombatComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

	bisAiming = false;
}

void UPlayerCombatComponent::EquipWeapon(APlayerWeapon* WeaponToEquip)
{
	if (Character && WeaponToEquip)
	{
		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetWeaponState(EPlayerWeaponState::EWS_Equipped);
		const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());

		}
		EquippedWeapon->SetOwner(Character);

	}
}

void UPlayerCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPlayerCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UPlayerCombatComponent, bisAiming);
}


// Called when the game starts
void UPlayerCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UPlayerCombatComponent::SetAiming(bool bIsAiming)
{
	bisAiming = bIsAiming;
	if (!Character->HasAuthority())
	{
		ServerSetAiming(bIsAiming);
	}
}

void UPlayerCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bisAiming = bIsAiming;
}


// Called every frame
void UPlayerCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

