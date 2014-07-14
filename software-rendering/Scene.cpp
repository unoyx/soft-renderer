#include "Scene.h"
#include <d3dx9mesh.h>
#include <assert.h>
#include "vector.h"
#include "Renderer.h"
#include "Logger.h"

using std::string;

void Scene::LoadFromXFile(string file, IDirect3DDevice9 *device)
{
    assert(device);

    ID3DXBuffer *material = nullptr;
    DWORD num_material = 0;
    ID3DXMesh *mesh = nullptr;

    HRESULT res = D3DXLoadMeshFromX(file.c_str(),     // pFilename
                                    D3DXMESH_DYNAMIC, // Options
                                    device,           // pD3DDevice
                                    nullptr,          // ppAdjacency
                                    &material,        // ppMaterials
                                    nullptr,          // ppEffectInstances
                                    &num_material,    // pNumMaterials
                                    &mesh);           // ppMesh
    if (FAILED(res))
    {
        Logger::GtLogError("load mesh from X file failed:%s", file.c_str());
        return;
    }
    assert(num_material == 1);
    D3DXMATERIAL *mat = static_cast<D3DXMATERIAL *>(material->GetBufferPointer());
    material_.ambient.r = mat->MatD3D.Ambient.r;
    material_.ambient.g = mat->MatD3D.Ambient.g;
    material_.ambient.b = mat->MatD3D.Ambient.b;
    material_.ambient.a = mat->MatD3D.Ambient.a;

    material_.diffuse.r = mat->MatD3D.Diffuse.r;
    material_.diffuse.g = mat->MatD3D.Diffuse.g;    
    material_.diffuse.b = mat->MatD3D.Diffuse.b;
    material_.diffuse.a = mat->MatD3D.Diffuse.a;

    material_.specular.r = mat->MatD3D.Specular.r;
    material_.specular.g = mat->MatD3D.Specular.g;
    material_.specular.b = mat->MatD3D.Specular.b;
    material_.specular.a = mat->MatD3D.Specular.a;

    material_.emissive.r = mat->MatD3D.Emissive.r;
    material_.emissive.g = mat->MatD3D.Emissive.g;
    material_.emissive.b = mat->MatD3D.Emissive.b;
    material_.emissive.a = mat->MatD3D.Emissive.a;

    material_.power = mat->MatD3D.Power;

    bool ret = texture_.Load(mat->pTextureFilename, device);
    assert(ret);
    material->Release();
    

    DWORD num_vertex = mesh->GetNumVertices();
    IDirect3DVertexBuffer9 *vertex_buffer = nullptr;
    res = mesh->GetVertexBuffer(&vertex_buffer);
    assert(SUCCEEDED(res));
    uint8 *vertex_data = nullptr;
    res = vertex_buffer->Lock(0, 0, reinterpret_cast<void **>(&vertex_data), D3DLOCK_READONLY);
    assert(SUCCEEDED(res));

    D3DVERTEXELEMENT9 vertex_elements[MAX_FVF_DECL_SIZE] = {0};
    res = mesh->GetDeclaration(vertex_elements);
    assert(SUCCEEDED(res));
    int offset_pos = 0;
    int offset_nor = 0;
    int offset_tex = 0;
    int vertex_size = mesh->GetNumBytesPerVertex();
    for (int i = 0; i < MAX_FVF_DECL_SIZE; ++i)
    {
        if (vertex_elements[i].Type == D3DDECLTYPE_UNUSED)
            break;
        if (vertex_elements[i].Usage == D3DDECLUSAGE_POSITION)
        {
            offset_pos = vertex_elements[i].Offset;
        }
        if (vertex_elements[i].Usage == D3DDECLUSAGE_NORMAL)
        {
            offset_nor =  vertex_elements[i].Offset;
        }
        if (vertex_elements[i].Usage == D3DDECLUSAGE_TEXCOORD)
        {
            offset_tex = vertex_elements[i].Offset;
        }
    }

    Primitive  verteies(num_vertex, nullptr, nullptr);
    for (int i = 0; i < num_vertex; ++i)
    {
        verteies.positions[i] = *reinterpret_cast<Vector3 *>(vertex_data + vertex_size * i + offset_pos);
        verteies.positions[i].Display();
        verteies.normals[i] = *reinterpret_cast<Vector3 *>(vertex_data + vertex_size * i + offset_nor);
        verteies.normals[i].Display();
        verteies.uvs[i] = *reinterpret_cast<Vector2 *>(vertex_data + vertex_size * i + offset_tex);
        verteies.uvs[i].Display();
    }
    primitive_ = verteies;

    vertex_buffer->Unlock();
    vertex_buffer->Release();
}


void Scene::Update(Renderer *renderer)
{
    renderer->DrawPrimitive(&primitive_);
}