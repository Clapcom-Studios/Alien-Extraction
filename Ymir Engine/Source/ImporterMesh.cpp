#include "ImporterMesh.h"

void ImporterMesh::Import(const aiMesh* mesh, Mesh* ourMesh)
{

}

uint ImporterMesh::Save(const Mesh* ourMesh, char** fileBuffer)
{
    //uint bufferSize = sizeof(uint) * 5 + sizeof(uint) * ourMesh->num_indices + sizeof(Vertex) * ourMesh->num_vertices;

    //// Asignar din�micamente el b�fer de archivos
    //*fileBuffer = new char[bufferSize];

    //// Utilizar un puntero para rastrear la posici�n actual en el b�fer de archivos
    //char* cursor = *fileBuffer;

    //// Guardar la cantidad de �ndices, v�rtices y otros datos en ranges
    //uint ranges[5] = { ourMesh->num_indices, ourMesh->num_vertices, 0, 0, 0 }; // Asumiendo que las otras propiedades son cero por ahora
    //bytes = sizeof(ranges);
    //memcpy(cursor, ranges, bytes);
    //cursor += bytes;

    //// Guardar los �ndices en el b�fer
    //bytes = sizeof(uint) * ourMesh->num_indices;
    //memcpy(cursor, ourMesh->indices, bytes);
    //cursor += bytes;

    //// Guardar los v�rtices en el b�fer
    //bytes = sizeof(Vertex) * ourMesh->num_vertices;
    //memcpy(cursor, ourMesh->vertices, bytes);

    //// Devolver el tama�o del b�fer
    //return bufferSize;
}

void ImporterMesh::Load(const char* fileBuffer, Mesh* ourMesh)
{

}
