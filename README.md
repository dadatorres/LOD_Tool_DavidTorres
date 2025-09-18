# 🛠️ ToolLODUpdater – Sistema de LODs en Unreal Engine

Este módulo implementa una **herramienta personalizada para gestionar y aplicar niveles de detalle (LODs)** a *Static Meshes* en **Unreal Engine** mediante **C++ y Blueprints**.  

Permite:
- Añadir LODs dinámicamente desde el editor.
- Configurar cada LOD desde la interfaz (sliders/UI).
- Aplicar los cambios directamente sobre el `StaticMesh` seleccionado, eliminando los LODs previos y generando los nuevos.

---

## 📂 Estructura del Código

### 1. `FLODSettings` (struct)
Definición de un LOD en la herramienta:

- `Reduction` → Porcentaje de reducción de polígonos (**0–100**).
- `ScreenSize` → Tamaño relativo de pantalla en el que este LOD será visible.
- `TargetActor` → Actor que contiene el *Static Mesh* al que se aplicará el LOD.

👉 Inicializado con valores seguros (`0.0f`, `nullptr`).

---

### 2. `UToolLODUpdater` (clase principal)
Hereda de `UUserWidget`, lo que permite exponer las funciones a **Blueprints/UI**.

#### 🔹 Variables
- `LODList` → Array de configuraciones `FLODSettings`, que almacena todos los LODs definidos desde la UI.

#### 🔹 Funciones principales
1. **`AddLOD(UObject* Target)`**  
   - Agrega una nueva configuración de LOD asociada a un `StaticMeshActor`.
   - Hace validaciones: actor válido, si tiene `StaticMeshComponent` y `StaticMesh`.
   - Muestra mensajes en **Output Log** y en pantalla.

2. **`UpdateLODFromUI(int32 Index, float Reduction, float ScreenSize)`**  
   - Actualiza un LOD existente con valores introducidos desde la UI.
   - Sobrescribe `Reduction` y `ScreenSize` en `LODList[Index]`.

3. **`ApplyLODs()`**  
   - Aplica todos los LODs definidos en `LODList` al `StaticMesh` asociado.
   - Pasos:
     1. Comprueba que exista al menos un LOD válido y que el target sea un `StaticMeshActor`.
     2. Elimina todos los LODs previos y crea el número necesario con `SetNumSourceModels`.
     3. Configura cada LOD:
        - `PercentTriangles` = `Reduction / 100` → Ejemplo: `50%` → `0.5`.
        - `MaxDeviation` = `0.0f`.
        - `WeldingThreshold` = `0.0f`.
        - `ScreenSize.Default` → Valor desde la UI.
     4. Reconstruye el mesh con:
        - `Mesh->Build()`
        - `Mesh->PostEditChange()`
        - `Mesh->MarkPackageDirty()`
   - Muestra un mensaje de éxito en pantalla.

---

## 📋 Flujo de Uso
1. Selecciona un `StaticMeshActor` en la escena.  
2. Desde el **Widget Blueprint (UI)**:
   - Llama a `AddLOD` para crear una entrada de LOD.
   - Ajusta sliders de **Reducción (%)** y **Screen Size** → se actualiza con `UpdateLODFromUI`.  
3. Pulsa un botón que ejecute `ApplyLODs`.  
   - Esto elimina todos los LODs anteriores y aplica los nuevos al mesh.  
4. Al moverte por la escena, el *Static Mesh* alternará entre LODs automáticamente.

---

## 📌 Notas importantes
- `Reduction` se maneja en **0–100** en la UI, pero en el código se convierte a **0.0–1.0** (`PercentTriangles`).  
- Cada ejecución de `ApplyLODs` **sobrescribe los LODs previos**.  
- Funciona únicamente con **StaticMeshActors** (no Skeletal Meshes).  
- Diseñado para **Unreal Engine 5.x**.

---

## ✅ Ejemplo de Log
```text
[ToolLODUpdater] ApplyLODs -> LOD 0 | PercentTriangles 0.50 | ScreenSize 0.43
[ToolLODUpdater] ApplyLODs -> LOD 1 | PercentTriangles 0.25 | ScreenSize 0.72
