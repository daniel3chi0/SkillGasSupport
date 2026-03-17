#include "SGSGameplayTags.h"
#include "GameplayTagsManager.h"

// 定义静态成员变量
FSGSGameplayTags FSGSGameplayTags::GameplayTags;

//
FGameplayTag FSGSGameplayTags::InitState_Spawned;
FGameplayTag FSGSGameplayTags::InitState_DataAvailable;
FGameplayTag FSGSGameplayTags::InitState_DataInitialized;
FGameplayTag FSGSGameplayTags::InitState_GameplayReady;

FGameplayTag FSGSGameplayTags::InputTag_Move;
FGameplayTag FSGSGameplayTags::InputTag_Look;

FGameplayTag FSGSGameplayTags::InputTag_ShowInvUI;
FGameplayTag FSGSGameplayTags::InputTag_MouseWheel;

FGameplayTag FSGSGameplayTags::InputTag_left;

FGameplayTag FSGSGameplayTags::InputTag_Combat_LeftAttack;

FGameplayTag FSGSGameplayTags::InputTag_Combat_Sword_Charge;
FGameplayTag FSGSGameplayTags::InputTag_Combat_Sword_Uncharge;
FGameplayTag FSGSGameplayTags::InputTag_Combat_Sword_ChargeAttack;
FGameplayTag FSGSGameplayTags::InputTag_Combat_Sword_UnchargeAttack;

FGameplayTag FSGSGameplayTags::InputTag_Combat_Bow;
FGameplayTag FSGSGameplayTags::InputTag_Combat_CancelBow;

FGameplayTag FSGSGameplayTags::InputTag_Combat_Dodge;
FGameplayTag FSGSGameplayTags::InputTag_Sprint;
FGameplayTag FSGSGameplayTags::InputTag_Jump;
FGameplayTag FSGSGameplayTags::InputTag_InfectPointInteract;
FGameplayTag FSGSGameplayTags::InputTag_WalkRun;

FGameplayTag FSGSGameplayTags::InputTag_TakeOutItem;
FGameplayTag FSGSGameplayTags::InputTag_DropItem;
FGameplayTag FSGSGameplayTags::InputTag_Combat_Archery_Charge;
FGameplayTag FSGSGameplayTags::InputTag_Combat_Archery_Trigger;
FGameplayTag FSGSGameplayTags::InputTag_Combat_Archery_Cancel;
FGameplayTag FSGSGameplayTags::InputTag_Skill_Perspective;

FGameplayTag FSGSGameplayTags::InputTag_Interactive_HolyWaterPlate;
FGameplayTag FSGSGameplayTags::InputTag_Interactive_Chest;

FGameplayTag FSGSGameplayTags::InputTag_Rescue;
FGameplayTag FSGSGameplayTags::InputTag_Suicide;

FGameplayTag FSGSGameplayTags::InputTag_ReturnHome;
FGameplayTag FSGSGameplayTags::InputTag_Respawn;

FGameplayTag FSGSGameplayTags::InputTag_SkillSystem_BattleSkill;
FGameplayTag FSGSGameplayTags::InputTag_SkillSystem_Magic;
FGameplayTag FSGSGameplayTags::InputTag_SkillSystem_VillainExtraBattleSkill1;
FGameplayTag FSGSGameplayTags::InputTag_SkillSystem_VillainExtraBattleSkill2;
FGameplayTag FSGSGameplayTags::InputTag_SkillSystem_DarkMagicSkillEquipOrUsed;
FGameplayTag FSGSGameplayTags::InputTag_SkillSystem_CancelSkillOrMagic;
FGameplayTag FSGSGameplayTags::Ability_ActivateFail_IsDead;


void FSGSGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& GameplayTagsManager =  UGameplayTagsManager::Get();

	GameplayTags.AddAllTags(GameplayTagsManager); 
	
}

void FSGSGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	AddTag(InitState_Spawned, "InitState.Spawned", "Actor/Component has initially spawned and can be extended");
	AddTag(InitState_DataAvailable, "InitState.DataAvailable", "All required data/assets has been loaded/replicated and is ready for initialization");
	AddTag(InitState_DataInitialized, "InitState.DataInitialized", "The available data/assets has been initialized for this actor/component,but it is not ready for full gameplay");
	AddTag(InitState_GameplayReady, "InitState.GameplayReady", "Actor/Component is fully ready for active gameplay");

	
	AddTag(InputTag_Move, "InputTag.Move", "Input Move");
	AddTag(InputTag_Look,"InputTag.Look","Input Look");

	AddTag(InputTag_ShowInvUI,"InputTag.ShowInvUI","Input ShowInvUI");
	AddTag(InputTag_MouseWheel,"InputTag.MouseWheel","Input MouseWheel");
	
	AddTag(InputTag_left, "InputTag.left", "InputTag left");
	
	AddTag(InputTag_Combat_LeftAttack,"InputTag.Combat.LeftAttack", "Input LeftAttack");
	AddTag(InputTag_Combat_Sword_Charge, "InputTag.Combat.Sword.Charge", "Input Sword Charge");
	AddTag(InputTag_Combat_Sword_Uncharge, "InputTag.Combat.Sword.Uncharge", "Input Sword Uncharge");
	AddTag(InputTag_Combat_Sword_ChargeAttack, "InputTag.Combat.Sword.ChargeAttack", "Input Sword ChargeAttack");
	AddTag(InputTag_Combat_Sword_UnchargeAttack, "InputTag.Combat.Sword.UnchargeAttack","Input Sword UnchargeAttack");

	AddTag(InputTag_Combat_Bow, "InputTag.Combat.Bow", "Input Bow");
	AddTag(InputTag_Combat_CancelBow, "InputTag.Combat.CancelBow", "Input CancelBow");

	AddTag(InputTag_Combat_Dodge, "InputTag.Combat.Dodge", "Input Dodge");
	AddTag(InputTag_Combat_Dodge, "InputTag.Sprint", "InputTag Sprint");
	AddTag(InputTag_Jump, "InputTag.Jump", "Input Jump");
	AddTag(InputTag_InfectPointInteract, "InputTag.InfectPointInteract", "Input InfectPointInteract");
	AddTag(InputTag_WalkRun, "InputTag.WalkRun", "Input WalkRun");
	
	AddTag(InputTag_Combat_Archery_Charge,"InputTag.Combat.Archery.Charge", "Input  Archery Charge");
	AddTag(InputTag_Combat_Archery_Trigger,"InputTag.Combat.Archery.Trigger", "Input  Archery Trigger");
	AddTag(InputTag_Combat_Archery_Cancel,"InputTag.Combat.Archery.Cancel", "Input  Archery Cancel");

	AddTag(InputTag_Skill_Perspective,"InputTag.Skill.Perspective","Input Skill Perspective");

	AddTag(InputTag_Interactive_HolyWaterPlate,"InputTag.Interactive.HolyWaterPlate","Input Interactive HolyWaterPlate");
	AddTag(InputTag_Interactive_HolyWaterPlate,"InputTag.Interactive.Chest","Input Interactive Chest");
	
	AddTag(InputTag_Rescue, "InputTag.Rescue", "Input Rescue");
	AddTag(InputTag_Suicide, "InputTag.Suicide", "Input Suicide");

	AddTag(InputTag_TakeOutItem,"InputTag.TakeOutItem", "Input TakeOutItem");
	AddTag(InputTag_DropItem,"InputTag.DropItem", "Input DropItem");
	
	AddTag(InputTag_ReturnHome, "InputTag.ReturnHome", "Input ReturnHome");
	AddTag(InputTag_Respawn,"InputTag.Respawn", "InputTag Respawn");
	
	AddTag(InputTag_SkillSystem_BattleSkill, "InputTag.SkillSystem.BattleSkill", "SkillSystem Battle Skill Input");
	AddTag(InputTag_SkillSystem_Magic, "InputTag.SkillSystem.Magic", "SkillSystem Magic Input");
	AddTag(InputTag_SkillSystem_VillainExtraBattleSkill1, "InputTag.SkillSystem.VillainExtraBattleSkill1", "SkillSystem VillainExtraBattleSkill1 Input");
	AddTag(InputTag_SkillSystem_VillainExtraBattleSkill2, "InputTag.SkillSystem.VillainExtraBattleSkill2", "SkillSystem VillainExtraBattleSkill2 Input");
	AddTag(InputTag_SkillSystem_DarkMagicSkillEquipOrUsed, "InputTag.SkillSystem.DarkMagicSkillEquipOrUsed", "SkillSystem DarkMagicSkillEquipOrUsed Input");
	AddTag(InputTag_SkillSystem_CancelSkillOrMagic, "InputTag.SkillSystem.CancelSkillOrMagic", "Tag cancel skill or magic in skill system");
	AddTag(Ability_ActivateFail_IsDead, "Ability.ActivateFail.IsDead", "Ability failed to activate because its owner is dead.");

}

	
void FSGSGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native) ")) + FString(TagComment));
}
