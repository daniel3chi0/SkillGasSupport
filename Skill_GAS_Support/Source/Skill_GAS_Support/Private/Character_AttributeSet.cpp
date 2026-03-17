// Fill out your copyright notice in the Description page of Project Settings.


#include "Character_AttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "Subsystem/SkillManager.h"
#include "Net/UnrealNetwork.h"

UCharacter_AttributeSet::UCharacter_AttributeSet()
{

	// //初始化
	// InitHealth(350.f);

	//*********逻辑需要不要注释!********
	InitMaxHealth(350.f);
	//
	// InitMana(200.f);
	// InitMaxMana(200.f);
	//
	// InitSpirit(540.f);
	// InitMaxSpirit(540.f);
	//
	// InitToughness(100.f);
	// InitMaxToughness(100.f);
	//
	// InitArmor(500.f);
	// InitMaxArmor(500.f);
	//
	// InitDownedHealth(100.f);
	// InitMaxDownedHealth(100.f);
	//
	
	
	
}

void UCharacter_AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//设置为复制
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacter_AttributeSet,Health,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacter_AttributeSet,MaxHealth,COND_None,REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UCharacter_AttributeSet,Mana,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacter_AttributeSet,MaxMana,COND_None,REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UCharacter_AttributeSet,Spirit,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacter_AttributeSet,MaxSpirit,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacter_AttributeSet,SpiritRegenRate,COND_None,REPNOTIFY_Always)

	DOREPLIFETIME_CONDITION_NOTIFY(UCharacter_AttributeSet,Toughness,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacter_AttributeSet,MaxToughness,COND_None,REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UCharacter_AttributeSet,Armor,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacter_AttributeSet,MaxArmor,COND_None,REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UCharacter_AttributeSet,DownedHealth,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacter_AttributeSet,MaxDownedHealth,COND_None,REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UCharacter_AttributeSet,MoveSpeed,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacter_AttributeSet,MaxMoveSpeed,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacter_AttributeSet, MinMoveSpeed, COND_None, REPNOTIFY_Always);
	
	
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacter_AttributeSet, Attack, COND_None, REPNOTIFY_Always);

	//体力
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacter_AttributeSet, Physical, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacter_AttributeSet, MaxPhysical, COND_None, REPNOTIFY_Always);

	//减伤率
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacter_AttributeSet, InjuryReduction, COND_None, REPNOTIFY_Always);

}

void UCharacter_AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{

	//Super::PreAttributeChange(Attribute, NewValue);
	//添加最大最小值限制
	if (Attribute == GetHealthAttribute())
	{
		const float TempMaxHealth = GetMaxHealth();
		NewValue = FMath::Clamp(NewValue, 0, TempMaxHealth);
	}
	if(Attribute == GetMaxHealthAttribute())
	{
		if (GetHealth() > NewValue)
		{
			UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
			ASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
		}
	}
	if (Attribute == GetManaAttribute())
	{
		const float TempMaxMana = GetMaxMana();
		NewValue = FMath::Clamp(NewValue, 0, TempMaxMana);
	}
	if (Attribute == GetSpiritAttribute())
	{
		const float TempMaxSpirit = GetMaxSpirit();
		NewValue = FMath::Clamp(NewValue, 0, TempMaxSpirit);
		//UE_LOG(LogSkillSystem, Error, TEXT("pre attribute change spirit = %f"), NewValue);
	}
	if (Attribute == GetMoveSpeedAttribute())
	{
		const float TempMaxMoveSpeed = GetMaxMoveSpeed();
		NewValue = FMath::Clamp(NewValue, 0, TempMaxMoveSpeed);
	}

	//在属性变化之前
	//限制最大体力
	if (Attribute == GetPhysicalAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxPhysical());
	}

	//限制当前速度的值
	if (Attribute == GetMoveSpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, GetMinMoveSpeed(), GetMaxMoveSpeed());
	}

	//选择减伤率的大小
	if (Attribute == GetInjuryReductionAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, 100);
	}

}

// void UCharacter_AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
// {
// 	Super::PreAttributeChange(Attribute, NewValue);
// 	if(Attribute == GetHealthAttribute())
// 	{
// 		NewValue = FMath::Clamp(NewValue,0.f,GetMaxHealth());
// 		UE_LOG(LogTemp,Warning,TEXT("Health:%f"),NewValue);
// 		
// 	}
// 	
// 	if(Attribute == GetSpiritAttribute())
// 	{
// 		NewValue = FMath::Clamp(NewValue,0.f,GetMaxSpirit());
// 		
// 	}
// 	
// 	
// 	
// }

void UCharacter_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayEffectContextHandle EffectContextHandle = Data.EffectSpec.GetContext();
	const UAbilitySystemComponent* SourceASC = EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if(IsValid(SourceASC)&& SourceASC->AbilityActorInfo->AvatarActor.Get())
	{
		AActor* SourceAvatarActor = SourceASC->AbilityActorInfo->AvatarActor.Get();
		const APlayerController* SourcePlayerController =SourceASC->AbilityActorInfo->PlayerController.Get();
		if(SourcePlayerController == nullptr && SourceAvatarActor!=nullptr)
		{
			if(const APawn* Pawn = Cast<APawn>(SourceAvatarActor))
			{
				SourcePlayerController = Cast<APlayerController>(Pawn->GetController());
				
				
			}
			if(SourcePlayerController)
			{
				ACharacter* SourceCharacter = Cast<ACharacter>(SourcePlayerController->GetPawn());
				
			}
			
		}	
		
	}
	if(Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		AActor* TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		
		
	}
	
	if(Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0, GetMaxHealth()));
	}
	else if(Data.EvaluatedData.Attribute == GetSpiritAttribute())
	{
		SetSpirit(FMath::Clamp(GetSpirit(), 0, GetMaxSpirit()));
	}
	else if(Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetPhysicalAttribute())
	{
		//在属性变化后，限制设置的最大和最小
		SetPhysical(FMath::Clamp(GetPhysical(), 0, GetMaxPhysical()));
	}
	else if(Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0, GetMaxMana()));
	}
	else if (Data.EvaluatedData.Attribute == GetMoveSpeedAttribute())
	{
		SetMoveSpeed(FMath::Clamp(GetMoveSpeed(), GetMinMoveSpeed(), GetMaxMoveSpeed()));

	}
	else if (Data.EvaluatedData.Attribute == GetInjuryReductionAttribute())
	{
		SetInjuryReduction(FMath::Clamp(GetInjuryReduction(), 0, 100));
	}
}

void UCharacter_AttributeSet::onRep_Health(const FGameplayAttributeData& OldHealth) const
{
	//预测和回调
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacter_AttributeSet,Health ,OldHealth );
	
}

void UCharacter_AttributeSet::onRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	//预测和回调
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacter_AttributeSet,MaxHealth ,OldMaxHealth );
}

void UCharacter_AttributeSet::onRep_Mana(const FGameplayAttributeData& OldMana) const
{
	//预测和回调
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacter_AttributeSet,Mana ,OldMana );
	
}

void UCharacter_AttributeSet::onRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	//预测和回调
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacter_AttributeSet,MaxMana ,OldMaxMana );
	
}

void UCharacter_AttributeSet::OnRep_ManaRegenRate(const FGameplayAttributeData& OldManaRegenRate) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacter_AttributeSet, ManaRegenRate, OldManaRegenRate);
}

void UCharacter_AttributeSet::onRep_Spirit(const FGameplayAttributeData& OldSpirit) const
{
	//预测和回调
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacter_AttributeSet,Spirit ,OldSpirit );
}

void UCharacter_AttributeSet::onRep_MaxSpirit(const FGameplayAttributeData& OldMaxSpirit) const
{
	//预测和回调
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacter_AttributeSet,MaxSpirit ,OldMaxSpirit );
}

void UCharacter_AttributeSet::OnRep_SpiritRegenRate(const FGameplayAttributeData& OldSpiritRegenRate) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacter_AttributeSet, SpiritRegenRate , OldSpiritRegenRate);
}

void UCharacter_AttributeSet::onRep_Toughness(const FGameplayAttributeData& OldToughness) const
{
	//预测和回调
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacter_AttributeSet,Toughness ,OldToughness );
}

void UCharacter_AttributeSet::onRep_MaxToughness(const FGameplayAttributeData& OldMaxToughness) const
{
	//预测和回调
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacter_AttributeSet,MaxToughness ,OldMaxToughness );
}

void UCharacter_AttributeSet::onRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	//预测和回调
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacter_AttributeSet,Armor ,OldArmor );	
	
}

void UCharacter_AttributeSet::onRep_MaxArmor(const FGameplayAttributeData& OldMaxArmor) const
{
	//预测和回调
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacter_AttributeSet,MaxArmor ,OldMaxArmor );
	
}

void UCharacter_AttributeSet::onRep_DownedHealth(const FGameplayAttributeData& OldDownedHealth) const
{
	//预测和回调
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacter_AttributeSet,DownedHealth ,OldDownedHealth );
	
}

void UCharacter_AttributeSet::onRep_MaxDownedHealth(const FGameplayAttributeData& OldMaxDownedHealth) const
{

	//预测和回调
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacter_AttributeSet,MaxDownedHealth ,OldMaxDownedHealth );
}

void UCharacter_AttributeSet::onRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed) const
{
	//预测和回调
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacter_AttributeSet,MoveSpeed ,OldMoveSpeed );
}

void UCharacter_AttributeSet::onRep_MaxMoveSpeed(const FGameplayAttributeData& OldMaxMoveSpeed) const
{
	//预测和回调
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacter_AttributeSet,MaxMoveSpeed ,OldMaxMoveSpeed );
}

void UCharacter_AttributeSet::onRep_MinMoveSpeed(const FGameplayAttributeData& OldMinMoveSpeed) const
{
	//预测和回调
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacter_AttributeSet, MinMoveSpeed, OldMinMoveSpeed);
}

void UCharacter_AttributeSet::OnRep_Attack(const FGameplayAttributeData& OlAttack) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacter_AttributeSet, Attack, OlAttack);
}

void UCharacter_AttributeSet::OnRep_Physical(const FGameplayAttributeData& OlPhysical) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacter_AttributeSet, Physical, OlPhysical);
}

void UCharacter_AttributeSet::OnRep_MaxPhysical(const FGameplayAttributeData& OlMaxPhysical) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacter_AttributeSet, MaxPhysical, OlMaxPhysical);
}

void UCharacter_AttributeSet::OnRep_InjuryReduction(const FGameplayAttributeData& OlInjuryReduction) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacter_AttributeSet, InjuryReduction, OlInjuryReduction);
}

void UCharacter_AttributeSet::InitAttributeSet()
{
	//初始化
	
	SetHealth(350.f);
	SetMaxHealth(350.f);
	
	SetMana(200.f);
	SetMaxMana(200.f);

	//SetSpirit(100.f);
	//SetMaxSpirit(100.f);
	SetSpiritRegenRate(-1.f);

	SetToughness(100.f);
	SetMaxToughness(100.f);

	SetArmor(500.f);
	SetMaxArmor(500.f);

	SetDownedHealth(100.f);
	SetMaxDownedHealth(100.f);
	
	SetPhysical(100.0f);
	SetMaxPhysical(100.0f);

	SetMinMoveSpeed(120.f);

	//设置减伤率
	SetInjuryReduction(0.f);
}

void UCharacter_AttributeSet::RecoverAttributeSet()
{
	//初始化
	
	SetHealth(350.f);
	SetMaxHealth(350.f);
	
	SetMana(200.f);
	SetMaxMana(200.f);

	SetSpirit(600.f);
	SetMaxSpirit(600.f);

	SetToughness(100.f);
	SetMaxToughness(100.f);

	SetArmor(500.f);
	SetMaxArmor(500.f);

	SetDownedHealth(100.f);
	SetMaxDownedHealth(100.f);

	SetPhysical(100.0f);
	SetMaxPhysical(100.0f);

	SetInjuryReduction(0.f);
}

void UCharacter_AttributeSet::SetEnemyAttribute(float Heal, float Arm)
{
	SetHealth(Heal);
	SetMaxHealth(Heal);

	SetArmor(Arm);
	SetMaxArmor(Arm);
}

