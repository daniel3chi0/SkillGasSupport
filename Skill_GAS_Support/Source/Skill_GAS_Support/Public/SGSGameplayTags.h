#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UGameplayTagsManager;


struct FSGSGameplayTags
{
public:
	static const FSGSGameplayTags& Get(){return  GameplayTags;};
	
	static void InitializeNativeTags();
	//初始化
	static FGameplayTag InitState_Spawned;
	static FGameplayTag InitState_DataAvailable;
	static FGameplayTag InitState_DataInitialized;
	static FGameplayTag InitState_GameplayReady;

	//基本输入
	static FGameplayTag InputTag_Move;
	static FGameplayTag InputTag_Look;

	//UI输入
	static FGameplayTag InputTag_ShowInvUI;					// 显示背包UI
	static FGameplayTag InputTag_MouseWheel;				// 切换物品栏
	
	//左键输入
	static FGameplayTag InputTag_left;
	
	//战斗输入
	static FGameplayTag InputTag_Combat_LeftAttack;
	
	static FGameplayTag InputTag_Combat_Archery_Charge;
	static FGameplayTag InputTag_Combat_Archery_Trigger;
	static FGameplayTag InputTag_Combat_Archery_Cancel;

	//右键蓄力
	static FGameplayTag InputTag_Combat_Sword_Charge;
	static FGameplayTag InputTag_Combat_Sword_Uncharge;
	static FGameplayTag InputTag_Combat_Sword_ChargeAttack;
	static FGameplayTag InputTag_Combat_Sword_UnchargeAttack;

	//弓箭、取消射箭
	static FGameplayTag InputTag_Combat_Bow;
	static FGameplayTag InputTag_Combat_CancelBow;

	//翻滚
	static FGameplayTag InputTag_Combat_Dodge;
	//冲刺
	static FGameplayTag InputTag_Sprint;

	//跳跃
	static FGameplayTag InputTag_Jump;
	//感染点交互
	static FGameplayTag InputTag_InfectPointInteract;
	//切换行走和奔跑
	static FGameplayTag InputTag_WalkRun;
	//技能输入
	static FGameplayTag InputTag_Skill_Perspective;
	
	//交互
	//交互圣水盘
	static FGameplayTag InputTag_Interactive_HolyWaterPlate;

	//交互宝箱
	static FGameplayTag InputTag_Interactive_Chest;

	//救援
	static FGameplayTag InputTag_Rescue;

	//自杀
	static FGameplayTag InputTag_Suicide;

	static FGameplayTag Ability_ActivateFail_IsDead;
	
	//物品
	static FGameplayTag InputTag_TakeOutItem;
	static FGameplayTag InputTag_DropItem;
	
	//回城
	static FGameplayTag InputTag_ReturnHome;
	//复活
	static FGameplayTag InputTag_Respawn;
	
	//技能系统
	//战技输入tag
	static FGameplayTag InputTag_SkillSystem_BattleSkill;

	//魔法输入
	static FGameplayTag InputTag_SkillSystem_Magic;

	//反派额外战技输入
	static FGameplayTag InputTag_SkillSystem_VillainExtraBattleSkill1;
	
	static FGameplayTag InputTag_SkillSystem_VillainExtraBattleSkill2;

	static FGameplayTag InputTag_SkillSystem_DarkMagicSkillEquipOrUsed;

	static FGameplayTag InputTag_SkillSystem_CancelSkillOrMagic;

protected:
	void AddAllTags(UGameplayTagsManager& Manager);
	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName,const ANSICHAR* TagComment);

private:
	static FSGSGameplayTags GameplayTags;
};