#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ToolLODUpdater.generated.h"  // <-- debe ser el último include en el header

USTRUCT(BlueprintType)
struct TOOL_LODS_DAVID_API FLODSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD")
    float Reduction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD")
    float ScreenSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD")
    UObject* TargetActor;

    // Constructor por defecto (más seguro para USTRUCTs)
    FLODSettings()
        : Reduction(0.f)
        , ScreenSize(0.f)
        , TargetActor(nullptr)
    {
    }
};

UCLASS(BlueprintType, Blueprintable)
class TOOL_LODS_DAVID_API UToolLODUpdater : public UUserWidget
{
    GENERATED_BODY()

public:


    /** Lista de configuraciones de LOD */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "LOD Tool")
    TArray<FLODSettings> LODList;

    /** Agregar un nuevo LOD */
    UFUNCTION(BlueprintCallable, Category = "LOD Tool")
    void AddLOD(UObject* Target);

    /** Aplicar los LODs almacenados al StaticMesh base */
    UFUNCTION(BlueprintCallable, Category = "LOD Tool")
    void ApplyLODs();

    /** Actualizar un LOD con valores venidos del UI */
    UFUNCTION(BlueprintCallable, Category = "LOD Tool")
    void UpdateLODFromUI(int32 Index, float Reduction, float ScreenSize);
};
