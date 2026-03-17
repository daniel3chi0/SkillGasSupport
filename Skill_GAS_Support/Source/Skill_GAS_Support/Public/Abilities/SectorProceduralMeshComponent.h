// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "SectorProceduralMeshComponent.generated.h"

class WESTERN_MAP_API UProceduralMeshHelper
{
public:

	static void CreateSectorMesh(FVector CenterPoint, FVector Forward, FVector Up, float Angle, float Radius, float Height, int32 SectionNum, TArray<FVector>& Out_Vertices, TArray<int32>& Out_Triangles);

	static TArray<FVector> CalSectorPoints(FVector CenterPoint, FVector ForwardVector, FVector UpVector, float Angle, float Radius, int32 SectionNum);

	static TArray<int32> CircleTriangles(int32 CenterIdx, int32 StartIdx, int32 EndIdx, bool bAnticlockwise);
	
	static TArray<int32> RectangleTriangles(int32 Leftdown, int32 Lefttop, int32 Rightdown, int32 Righttop);
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew)
class WESTERN_MAP_API UProceduralMeshParam_Sector : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CenterPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Forward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Up;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Angle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Height;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SectionNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDebug;
	
public:
	UProceduralMeshParam_Sector(){}
	
	UFUNCTION(BlueprintCallable)
	void Init(TArray<FVector>& Out_Vertices, TArray<int32>& Out_Triangles);

	FVector UpdateParam(float InCubeLength, float InCubeWidth, float InCubeHeight);

	void CreateSectorMesh(TArray<FVector>& Out_Vertices, TArray<int32>& Out_Triangles, FVector CenterPoint, FVector Forward, FVector Up, float Angle, float Radius, float Height, int32 SectionNum);
};
/**
 * 
 */
UCLASS(meta=(BlueprintSpawnableComponent), ClassGroup = "Collision")
class WESTERN_MAP_API UCustomProceduralMeshComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:
	TArray<FVector> Vertices;
	
	TArray<int32> Triangles;

	UPROPERTY(EditAnywhere)
	bool bDebug;

public:
	UFUNCTION(BlueprintCallable)
	void Init(UProceduralMeshParam_Sector* InProceduralMeshParam);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void DrawDebug();

	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
};
