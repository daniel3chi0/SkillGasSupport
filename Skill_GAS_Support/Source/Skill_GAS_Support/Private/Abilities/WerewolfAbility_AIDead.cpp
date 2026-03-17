// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/WerewolfAbility_AIDead.h"
#include <Component/Werewolf_CombatComponent.h>
#include <AIController.h>
#include "BrainComponent.h"
#include <Manager/CharacterManager/Werewolf_AIManager.h>
#include <Abilities/Tasks/AbilityTask_WaitDelay.h>



void UWerewolfAbility_AIDead::GeneralStop()
{
	AActor* TempActor = GetAvatarActorFromActorInfo();
	if (!TempActor)
	{
		return;
	}
	APawn* TempPawn = Cast<APawn>(TempActor);
	//停止所有逻辑
	if (TempPawn)
	{
		if (AAIController* TempAIController = Cast<AAIController>(TempPawn->GetController()))
		{
			const FString& Reason = "";

			if (UBrainComponent* TempBrainComponent = TempAIController->GetBrainComponent())
			{
				TempBrainComponent->StopLogic(Reason);
			}
			TempAIController->ClearFocus(EAIFocusPriority::Gameplay);

		}

	}

	//停止时间轴并开启模拟
	if (UWerewolf_CombatComponent* TempCBC = TempActor->GetComponentByClass<UWerewolf_CombatComponent>())
	{
		TempCBC->StopTimelineAndStartSimulate();
	}
	

	//最后会换成播放的蒙太奇
	MontageStop();

	


	



	//掉落
	if (UWerewolf_AIManager* AIManager =  TempActor->GetWorld()->GetSubsystem<UWerewolf_AIManager>())
	{

		//掉落
		AIManager->DropItems(TempActor);

	}

	
	K2_PreMove();

	// 
	UAbilityTask_WaitDelay* DelayTask = UAbilityTask_WaitDelay::WaitDelay(this, PreMoveDelayTime); // 设置延迟秒数
	if (DelayTask)
	{
		DelayTask->OnFinish.AddDynamic(this, &UWerewolfAbility_AIDead::MoveToCemetery);
		DelayTask->ReadyForActivation();
	}
	
}

void UWerewolfAbility_AIDead::MoveToCemetery()
{
	AActor* TempActor = GetAvatarActorFromActorInfo();
	if (!TempActor) return;

	APawn* TempPawn = Cast<APawn>(TempActor);
	if (!TempPawn) return;

	// 移动到坟场
	FVector TempVector(0, 0, 0);
	TempPawn->SetActorLocation(TempVector);

	// 
	UAbilityTask_WaitDelay* DelayTask = UAbilityTask_WaitDelay::WaitDelay(this, PostMoveDelayTime); // 设置延迟秒数
	if (DelayTask)
	{
		DelayTask->OnFinish.AddDynamic(this, &UWerewolfAbility_AIDead::PostMove);
		DelayTask->ReadyForActivation();
	}
}

void UWerewolfAbility_AIDead::PostMove()
{
	AActor* TempActor = GetAvatarActorFromActorInfo();
	if (!TempActor) return;

	APawn* TempPawn = Cast<APawn>(TempActor);
	if (!TempPawn) return;
	
	K2_PostMove();

	//清除AI管理器中的指针
	if (UWerewolf_AIManager* AIManager =  TempActor->GetWorld()->GetSubsystem<UWerewolf_AIManager>())
	{
		//消除指针
		AIManager->AIDead(TempActor);
	}

	if (AController* TempController = TempPawn->GetController())
	{
		TempController->UnPossess();
		TempController->Destroy();
	}

	TempPawn->Destroy();

	K2_EndAbility();
}
