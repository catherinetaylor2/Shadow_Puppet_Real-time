#ifndef readObj_hpp
#define readObj_hpp

#include <string>

class ObjFile{
    public:
        ObjFile(std::string name);
        void get_vertices(float** vertices);
        void get_normals(float** normals);
        void get_texture(float** texture_coords);
        void get_face_data(int** face_vertex, int** face_normals, int** face_textures);
        static void clean_up(float* vertices, float* normals, float* texture_coords,int* face_vertex, int* face_normals, int* face_textures);
        int get_NumberOfFaces(void){
            return NumberOfFaces;
        }
        int get_NumberOfVertices(void){
            return NumberOfVertices;
        }
        std::string get_file_name(void){
            return ObjFileName;
        }
        bool doesExist(void){
            return exist;
        }
        void get_mesh_data(ObjFile mesh, int** face_vertex, int** face_normals, int** face_textures, float** texture_coords, float** normals, float** vertices, int* NumberOfFaces, int* NumberOfVertices);
    private:
		std::string ObjFileName;
        int NumberOfNormals;
        int NumberOfVertices;
        int NumberOfFaces;
        bool exist;
};
#endif