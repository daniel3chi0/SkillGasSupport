// Fill out your copyright notice in the Description page of Project Settings.
#include "Abilities/SectorProceduralMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"

//扇形体
/*
*	创建扇形体（圆柱体）绘制信息;
*	@CenterPoint 扇形体（圆柱体）的中心点;
*	@Forward 朝向；
*	@Up 朝上方向；
*	@Angle 扇形角度，360表示为圆柱体;
*	@Radius 扇形半径;
*	@Height 扇形体（圆柱体）的高度
*	@SectionNum 扇形体（圆柱体）的分割面数。数值越大，越圆滑。
*	@Out_Vertices 输出的顶点；
*	@Out_Triangles 输出的三角形；
*/

/*
*	=========== 有bug 暂时用不了 ===========
*/
 void UProceduralMeshHelper::CreateSectorMesh(FVector CenterPoint, FVector Forward, FVector Up, float Angle, float Radius, float Height, int32 SectionNum, TArray<FVector>& Out_Vertices, TArray<int32>& Out_Triangles)
 {
     //上下2片扇形，左右2个矩形，前面多个碎面;
     //1、算出所有的顶点;
     Forward.Normalize();
     Up.Normalize();
     Angle = Angle > 360.f ? 360.f : Angle;
     FVector lowPeakPoint = CenterPoint - Up * (Height / 2.0f);
     FVector highPeakPoint = CenterPoint + Up * (Height / 2.0f);
     TArray<FVector> lowPoints = CalSectorPoints(lowPeakPoint, Forward, Up, Angle, Radius, SectionNum);
     TArray<FVector> highPoints = CalSectorPoints(highPeakPoint, Forward, Up, Angle, Radius, SectionNum);
 	
     Out_Vertices.Add(lowPeakPoint);
     Out_Vertices.Append(lowPoints);
     int32 LowStartIdx = 0;
     int32 LowEndIdx = Out_Vertices.Num() - 1;
     int32 HighStartIdx = Out_Vertices.Num();
 
     Out_Vertices.Add(highPeakPoint);
     Out_Vertices.Append(highPoints);
     int32 HighEndIdx = Out_Vertices.Num() - 1;
 
     //2.1 下面的三角形
     TArray<int32> tmplowTriangle = CircleTriangles(LowStartIdx, LowStartIdx + 1, LowEndIdx, false);
     Out_Triangles.Append(tmplowTriangle);
 
     //2.2 上面的三角形
     TArray<int32> tmphightTriangle = CircleTriangles(HighStartIdx, HighStartIdx + 1, HighEndIdx, true);
     Out_Triangles.Append(tmphightTriangle);
 
     //如果是圆柱形的话不需要生成侧面的矩形
     if (!FMath::IsNearlyEqual(Angle, 360.f))
     {
         //2.3 左边矩形 
         TArray<int32> tmpleftRectangle = RectangleTriangles(LowStartIdx + 1, HighStartIdx + 1, LowStartIdx, HighStartIdx);
         Out_Triangles.Append(tmpleftRectangle);
 
         //2.4 右边矩形
         TArray<int32> tmprightRectangle = RectangleTriangles(LowStartIdx, HighStartIdx, LowEndIdx, HighEndIdx);
         Out_Triangles.Append(tmprightRectangle);
     }
 
     //2.5 前面碎面，注意法线方向
     for (int32 idx = 1; idx < LowEndIdx; idx++)
     {
         int32 tmprightdown = idx;
         int32 tmpleftdown = idx + 1;
         int32 tmprighttop = HighStartIdx + tmprightdown;
         int32 tmplefttop = HighStartIdx + tmpleftdown;
         TArray<int32> tmpChips = RectangleTriangles(tmpleftdown, tmplefttop, tmprightdown, tmprighttop);
         Out_Triangles.Append(tmpChips);
     }
 }
 
 TArray<FVector> UProceduralMeshHelper::CalSectorPoints(FVector CenterPoint, FVector ForwardVector, FVector UpVector, float Angle, float Radius, int32 SectionNum)
 {
     TArray<FVector> ResArray;
     ForwardVector.Normalize();
     UpVector.Normalize();
 
     float SectionAngle = SectionNum > 0 ? Angle / SectionNum : Angle;
     FVector SectionForward = ForwardVector.RotateAngleAxis(-1 * (Angle / 2), UpVector) * Radius;
     FVector leftpoint = CenterPoint + SectionForward;
     ResArray.Add(leftpoint);
     for (int32 idx = 0; idx < SectionNum; idx++)
     {
         SectionForward = SectionForward.RotateAngleAxis(SectionAngle, UpVector);
         FVector tmpPoint = CenterPoint + SectionForward;
         ResArray.Add(tmpPoint);
     }
     return ResArray;
 }

//圆或者扇形
TArray<int32> UProceduralMeshHelper::CircleTriangles(int32 CenterIdx, int32 StartIdx, int32 EndIdx, bool bAnticlockwise)
 {
 	TArray<int32> Triangles;
 	
	 //CenterIdx是圆的中心点下标
	 for (int32 Idx = StartIdx; Idx < EndIdx; Idx++)
	 {
		if (bAnticlockwise)
		{
		 	//逆时针构造;
		 	Triangles.Add(CenterIdx);
		 	Triangles.Add(Idx + 1);
		 	Triangles.Add(Idx);
		}
		else
		{
		 	//顺时针构造;
		 	Triangles.Add(CenterIdx);
		 	Triangles.Add(Idx);
		 	Triangles.Add(Idx + 1);
		}
	 }

	 return Triangles;
 }

//矩形
TArray<int32> UProceduralMeshHelper::RectangleTriangles(int32 Leftdown, int32 Lefttop, int32 Rightdown, int32 Righttop)
 {
 	TArray<int32> Triangles;
 	
 	Triangles.Add(Rightdown);
 	Triangles.Add(Righttop);
 	Triangles.Add(Lefttop);
 	
 	Triangles.Add(Rightdown);
 	Triangles.Add(Lefttop);
 	Triangles.Add(Leftdown);
 	
 	return Triangles;
 }

void UProceduralMeshParam_Sector::Init(TArray<FVector>& Out_Vertices, TArray<int32>& Out_Triangles)
{
	CreateSectorMesh(Out_Vertices, Out_Triangles, CenterPoint, Forward, Up, Angle, Radius, Height, SectionNum);
}

FVector UProceduralMeshParam_Sector::UpdateParam(float InCubeLength, float InCubeWidth, float InCubeHeight)
{
	/*CurCubeLength = InCubeLength;
	CurCubeWidth = InCubeWidth;
	CurCubeHeight = InCubeHeight;
	
	float LengthScale = CubeLength == 0.f ? 1.0f : CurCubeLength / CubeLength;
	float WidthScale = CubeWidth == 0.f ? 1.0f : CurCubeWidth / CubeWidth;
	float HeightScale = CubeHeight == 0.f ? 1.0f : CurCubeHeight / CubeHeight;*/
	
	return FVector(1, 1, 1);
}

void UProceduralMeshParam_Sector::CreateSectorMesh(TArray<FVector>& Out_Vertices, TArray<int32>& Out_Triangles, FVector InCenterPoint, FVector InForward, FVector InUp, float InAngle, float InRadius, float InHeight, int32 InSectionNum)
{
	/*TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;*/
	//UProceduralMeshHelper::CreateCubeMesh(CubeRaduis, Out_Vertices, Out_Triangles);
 	UProceduralMeshHelper::CreateSectorMesh(InCenterPoint, InForward, InUp, InAngle, InRadius, InHeight, InSectionNum, Out_Vertices, Out_Triangles);
}

void UCustomProceduralMeshComponent::Init(UProceduralMeshParam_Sector* InProceduralMeshParam)
{
 	if (!InProceduralMeshParam)
 	{
 		return;
 	}
 	//创建新的之前清空一下现有信息
 	ClearAllMeshSections();
 	//注意要清空已生成的碰撞信息，不然会残留过期信息
 	ClearCollisionConvexMeshes();
 	Vertices.Reset();
 	Triangles.Reset();
 	InProceduralMeshParam->Init(Vertices, Triangles);

 	TArray<FVector2D> EmptyArray;
 	TArray<FVector> Normals;
 	TArray<FVector2D> UV0;
 	TArray<FLinearColor> VertexColors;
 	TArray<FProcMeshTangent> Tangents;
 	
 	bUseComplexAsSimpleCollision = false;
 	CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, true);
 	AddCollisionConvexMesh(Vertices);

 	if (InProceduralMeshParam->bDebug)
 	{
 		PrimaryComponentTick.bCanEverTick = true;
 	}
}

void UCustomProceduralMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

 	if (bDebug)
 	{
 		//这里控制的是运行时的Debug显示，使用的是
 		DrawDebug();
 	}
}

void UCustomProceduralMeshComponent::DrawDebug()
 {
 	FVector ComponentLocation = GetComponentLocation();
 	for (int32 Idx = 0; Idx < Triangles.Num() - 2; Idx++)
 	{
 		if (Triangles[Idx] > Vertices.Num() - 1)
 		{
 			continue;
 		}
 		UKismetSystemLibrary::DrawDebugLine(this, Vertices[Triangles[Idx]], Vertices[Triangles[Idx + 1]], FLinearColor::Green, 1.0f);
 		UKismetSystemLibrary::DrawDebugLine(this, Vertices[Triangles[Idx + 1]], Vertices[Triangles[Idx + 2]], FLinearColor::Green, 1.0f);
 		UKismetSystemLibrary::DrawDebugLine(this, Vertices[Triangles[Idx + 2]], Vertices[Triangles[Idx]], FLinearColor::Green, 1.0f);
 	}
 }

FPrimitiveSceneProxy* UCustomProceduralMeshComponent::CreateSceneProxy()
 {
 	//SCOPE_CYCLE_COUNTER(STAT_ProcMesh_CreateSceneProxy);
 	class FCustomProceduralMeshSceneProxy final : public FPrimitiveSceneProxy
	{
	public:
		SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}

		FCustomProceduralMeshSceneProxy(UCustomProceduralMeshComponent* Component)
			: FPrimitiveSceneProxy(Component)
			, Vertices(Component->Vertices)
			, Triangles(Component->Triangles)
 			, OwnerComponent(Component)
		{
		}

		virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
		{
			//QUICK_SCOPE_CYCLE_COUNTER(STAT_BoxSceneProxy_GetDynamicMeshElements);
			if (OwnerComponent == nullptr)
			{
				return;
			}

			const FMatrix& LocalToWorld = GetLocalToWorld();

			FVector ComponentLocation = OwnerComponent->GetComponentLocation();
			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (VisibilityMap & (1 << ViewIndex))
				{
					const FSceneView* View = Views[ViewIndex];
					
	                 //可以自定义绘制颜色
					//const FLinearColor DrawColor = GetViewSelectionColor(FColor(223, 149, 157, 255), *View, IsSelected(), IsHovered(), false, IsIndividuallySelected());

					FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);
					for (int32 Idx = 0; Idx < Triangles.Num() - 2; Idx++)
					{
						if (Triangles[Idx] > Vertices.Num() - 1)
						{
							continue;
						}
						
						DrawDashedLine(PDI, Vertices[Triangles[Idx]], Vertices[Triangles[Idx + 1]], FLinearColor::Green, 1.0f, 1);
						DrawDashedLine(PDI, Vertices[Triangles[Idx + 1]], Vertices[Triangles[Idx + 2]], FLinearColor::Green, 1.0f, 1);
						DrawDashedLine(PDI, Vertices[Triangles[Idx + 2]], Vertices[Triangles[Idx]], FLinearColor::Green, 1.0f, 1);
					}
				}
			}
		}

		virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const
		{
			FPrimitiveViewRelevance Result;
			Result.bDrawRelevance = IsShown(View);
			Result.bShadowRelevance = IsShadowCast(View);
			Result.bDynamicRelevance = true;
			Result.bRenderInMainPass = ShouldRenderInMainPass();
			Result.bUsesLightingChannels = GetLightingChannelMask() != GetDefaultLightingChannelMask();
			Result.bRenderCustomDepth = ShouldRenderCustomDepth();
			Result.bTranslucentSelfShadow = bCastVolumetricTranslucentShadow;
			//MaterialRelevance.SetPrimitiveViewRelevance(Result);
			Result.bVelocityRelevance = IsMovable() && Result.bOpaque && Result.bRenderInMainPass;
			return Result;
		}

		virtual uint32 GetMemoryFootprint(void) const
		{
			return(sizeof(*this) + GetAllocatedSize());
		}

	private:
		TArray<FVector> Vertices;
		TArray<int32> Triangles;

		UCustomProceduralMeshComponent* OwnerComponent;
	};

 	return new FCustomProceduralMeshSceneProxy(this);
 }
