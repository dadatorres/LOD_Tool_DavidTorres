#include "ToolLODUpdater.h"
#include "Engine/Engine.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

void UToolLODUpdater::AddLOD(UObject* Target)
{
    FLODSettings NewLOD;
    NewLOD.Reduction = 0.0f;
    NewLOD.ScreenSize = 0.0f;
    NewLOD.TargetActor = Target;

    LODList.Add(NewLOD);

    FString ActorName = TEXT("NULL");

    if (Target)
    {
        ActorName = GetNameSafe(Target);

        if (AStaticMeshActor* MeshActor = Cast<AStaticMeshActor>(Target))
        {
            if (UStaticMeshComponent* MeshComp = MeshActor->GetStaticMeshComponent())
            {
                if (UStaticMesh* Mesh = MeshComp->GetStaticMesh())
                {
                    UE_LOG(LogTemp, Display, TEXT("[ToolLODUpdater] AddLOD -> Actor '%s' con mesh '%s' | Total LODs: %d"),
                        *ActorName,
                        *GetNameSafe(Mesh),
                        LODList.Num());

                    if (GEngine)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green,
                            FString::Printf(TEXT("LOD añadido a %s (mesh: %s)"), *ActorName, *Mesh->GetName()));
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("[ToolLODUpdater] AddLOD -> '%s' tiene StaticMeshComponent pero sin mesh asignado"), *ActorName);
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("[ToolLODUpdater] AddLOD -> '%s' no tiene StaticMeshComponent"), *ActorName);
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[ToolLODUpdater] AddLOD -> '%s' no es StaticMeshActor"), *ActorName);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[ToolLODUpdater] AddLOD -> NULL | Total LODs: %d"),
            LODList.Num());

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow,
                FString::Printf(TEXT("LOD añadido sin actor (total: %d)"), LODList.Num()));
        }
    }
}

void UToolLODUpdater::UpdateLODFromUI(int32 Index, float Reduction, float ScreenSize)
{
    if (LODList.IsValidIndex(Index))
    {
        LODList[Index].Reduction = Reduction;
        LODList[Index].ScreenSize = ScreenSize;

        UE_LOG(LogTemp, Display, TEXT("[ToolLODUpdater] UpdateLODFromUI -> Index %d | Reduction %.2f | ScreenSize %.2f"),
            Index, Reduction, ScreenSize);
    }
}

void UToolLODUpdater::ApplyLODs()
{
    if (LODList.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ToolLODUpdater] ApplyLODs llamado pero no hay LODs en la lista"));
        return;
    }

    UObject* Target = LODList[0].TargetActor;
    if (!Target)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ToolLODUpdater] ApplyLODs -> Target nulo en LODList[0]"));
        return;
    }

    AStaticMeshActor* MeshActor = Cast<AStaticMeshActor>(Target);
    if (!MeshActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ToolLODUpdater] ApplyLODs -> Target no es StaticMeshActor"));
        return;
    }

    UStaticMeshComponent* MeshComp = MeshActor->GetStaticMeshComponent();
    if (!MeshComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ToolLODUpdater] ApplyLODs -> StaticMeshActor sin StaticMeshComponent"));
        return;
    }

    UStaticMesh* Mesh = MeshComp->GetStaticMesh();
    if (!Mesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ToolLODUpdater] ApplyLODs -> StaticMeshComponent sin StaticMesh asignado"));
        return;
    }

    // 🔹 Elimina LODs previos y crea los nuevos
    Mesh->SetNumSourceModels(LODList.Num());

    for (int32 LODIndex = 0; LODIndex < LODList.Num(); LODIndex++)
    {
        FLODSettings LODConfig = LODList[LODIndex];
        FStaticMeshSourceModel& SrcModel = Mesh->GetSourceModel(LODIndex);

        // 🔹 Configuración de reducción
        SrcModel.ReductionSettings.PercentTriangles = FMath::Clamp(LODConfig.Reduction / 100.f, 0.f, 1.f);
        SrcModel.ReductionSettings.MaxDeviation = 0.0f;   // usar PercentTriangles 
        SrcModel.ReductionSettings.WeldingThreshold = 0.0f;


        // 🔹 ScreenSize
        SrcModel.ScreenSize.Default = FMath::Clamp(LODConfig.ScreenSize, 0.f, 4.f);

        UE_LOG(LogTemp, Display, TEXT("[ToolLODUpdater] ApplyLODs -> LOD %d | PercentTriangles %.2f | ScreenSize %.2f"),
            LODIndex, SrcModel.ReductionSettings.PercentTriangles, SrcModel.ScreenSize.Default);
    }

    // 🔹 Reconstruye el StaticMesh
    Mesh->Build();
    Mesh->PostEditChange();
    Mesh->MarkPackageDirty();

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green,
            FString::Printf(TEXT("LODs aplicados al mesh %s"), *Mesh->GetName()));
    }
}
