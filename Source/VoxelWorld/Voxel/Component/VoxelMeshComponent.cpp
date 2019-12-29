// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelMeshComponent.h"
#include "../Util/VoxelUtil.h"
#include "../Worker/VoxelTerrainWorker.h"

UVoxelMeshComponent::UVoxelMeshComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickInterval = 0.1f;
	bUseAsyncCooking = true;

	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialObject(TEXT("Material'/Game/VoxelWorld/Material/M_Voxel'"));

	if (MaterialObject.Object)
	{
		SetMaterial(0, MaterialObject.Object);
	}

	SetMobility(EComponentMobility::Static);
}

void UVoxelMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (MeshQueue.IsEmpty())
	{
		PrimaryComponentTick.SetTickFunctionEnable(false);
		return;
	}

	FTerrainWorkerInformation Information;
	MeshQueue.Dequeue(Information);
	CreateMeshSection_LinearColor(0, Information.Vertices, Information.Indices, Information.Normals, Information.UV0, Information.UV1, Information.UV2, Information.UV3, Information.VertexColors, Information.Tangents, true);
}

void UVoxelMeshComponent::GenerateVoxelMesh(const TArray<FVoxel> Voxels, FIntVector ChunkSize, float ChunkScale)
{
	FTerrainWorkerInformation Information;

	Information.Voxels = Voxels;
	Information.MeshComponent = this;
	Information.ChunkSize = ChunkSize;
	Information.ChunkScale = ChunkScale;

	FVoxelTerrainWorker::Enqueue(Information);
}

void UVoxelMeshComponent::FinishWork(FTerrainWorkerInformation Information)
{
	PrimaryComponentTick.SetTickFunctionEnable(true);
	MeshQueue.Enqueue(Information);
}