#include "ObjLoader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

/**bool loadObject(const char * filename, std::vector<point4> &outVertices, std::vector<int> &outIndices,std::vector<point3> &outNormals, std::vector<point2> &outUv)
{
	std::vector<GLushort> elements;
//	std::ifstream in(filename, std::ifstream::in);
	// if (!in) { std::cerr << "Cannot open " << filename << std::endl; exit(1); }
	std::string line;
	FILE * file = fopen(filename, "r");
if( file == NULL ){
    printf("Impossible to open the file !\n");
    return false;
	}
	while( 1 ){
 
    char lineHeader[256];
    // read the first word of the line
    int res = fscanf(file, "%s", lineHeader);
    if (res == EOF)
        break; // EOF = End Of File. Quit the loop.
	
	if ( strcmp( lineHeader, "v" ) == 0 ){
    point3 vertex;
    fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
    outVertices.push_back(vertex);
	}
	else if ( strcmp( lineHeader, "vt" ) == 0 ){
    point2 uv;
    fscanf(file, "%f %f\n", &uv.x, &uv.y );
    outUv.push_back(uv);
	}
	else if ( strcmp( lineHeader, "vn" ) == 0 ){
    point3 normal;
    fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
    outNormals.push_back(normal);
	}
	else if ( strcmp( lineHeader, "f" ) == 0 ){
    std::string vertex1, vertex2, vertex3;
    unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];
    int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2], &vertexIndex[3], &uvIndex[3], &normalIndex[3] );
   // if (matches != 9 || matches!=12){
  //      printf("File can't be read by our simple parser : ( Try exporting with other options\n");
 //       return false;
//    }
   outVertices.push_back(vertexIndex[0]);
   outVertices.push_back(vertexIndex[1]);
    outVertices.push_back(vertexIndex[2]);

		outVertices.push_back(vertexIndex[3]);
    outUv.push_back(uvIndex[0]);
    outUv.push_back(uvIndex[1]);
    outUv.push_back(uvIndex[2]);

		outUv.push_back(uvIndex[3]);
    outNormals.push_back(normalIndex[0]);
    outNormals.push_back(normalIndex[1]);
    outNormals.push_back(normalIndex[2]);

			outNormals.push_back(normalIndex[3]);
	}
	else if ( strcmp( lineHeader, "#" ) == 0 ){}
	else{
		//do nothing
	}
	}
	fclose(file);
	return true;
}*/

void printDebug(std::vector<point4> &positions, std::vector<int> &indices);

point4 toVec4(std::istringstream &iss){
	float x,y,z,w;
	iss >> x;
	iss >> y;
	iss >> z;
	iss >> w;
	return point4(x,y,z,w);
}
point3 toVec3(std::istringstream &iss){
	float x,y,z;
	iss >> x;
	iss >> y;
	iss >> z;
	return point3(x,y,z);
}

point2 toVec2(std::istringstream &iss){
	float x,y;
	iss >> x;
	iss >> y;
	return point2(x,y);
}

struct TriangleIndex{
	int position;
	int normal;
	int uv;

	bool replace(std::string& str, const std::string& from, const std::string& to) {
		size_t start_pos = str.find(from);
		if(start_pos == std::string::npos)
			return false;
		str.replace(start_pos, from.length(), to);
		return true;
	}

	TriangleIndex(std::string p):position(-1),normal(-1),uv(-1) {
		// position/uv/normal
		replace(p, "//","/-1/");
		std::stringstream ss(p);
		char buffer[50];

		ss.getline(buffer,50, '/');
		position = atoi(buffer);
		if (ss.good()){
			ss.getline(buffer,50, '/');
			uv = atoi(buffer);
		}
		if (ss.good()){
			ss.getline(buffer,50, '/');
			normal = atoi(buffer);
		}
	}

	// needed to use TriangleIndex as key in map
	bool operator <(const TriangleIndex& Rhs) const {
		return (position < Rhs.position) ||
			(position == Rhs.position && normal < Rhs.normal) ||
			(position == Rhs.position && normal == Rhs.normal && uv < Rhs.uv);      
	}
};

struct TriangleString{
	TriangleIndex v0;
	TriangleIndex v1;
	TriangleIndex v2;

	TriangleString(std::string v0, std::string v1, std::string v2):v0(v0),v1(v1),v2(v2){
	}

	TriangleIndex get(int index){
		if (index == 0) {
			return v0;
		} else if (index == 1) {
			return v1;
		} 
		return v2;
	}
};

bool loadObject(const char * filename, 
	std::vector<point4> &outPositions, 
	std::vector<int> &outIndices,
	std::vector<point3> &outNormal, 
	std::vector<point2> &outUv,
	float scale){
	
	std::vector<point4> positions;
	std::vector<point3> normals;
	std::vector<point2> uvs;


	std::vector<TriangleString> triangles;
	std::ifstream ifs ( filename , std::ifstream::in );
	char buffer[512];
	while (ifs.good()){
		ifs.getline(buffer,512);
		
		std::string line(buffer);
		std::istringstream iss(line);
		std::string token;
		iss >> token;
		if (token.compare("o")==0){
			// does not support multiple objects
		} else if (token.compare("usemtl")==0){
			// does not support multiple materials
		} else if (token.compare("v")==0){
			positions.push_back(toVec3(iss));
		} else if (token.compare("vn")==0){
			normals.push_back( toVec3(iss));
		} else if (token.compare("vt")==0){
			uvs.push_back( toVec2(iss));
		} else if (token.compare("f")==0){
			std::vector<std::string> polygon;
			do {
				std::string index;
				iss >> index;
				if (index.length() > 0) {
					polygon.push_back(index);
				}
			} while (iss);

			// triangulate pologon
			TriangleString triangle(polygon[0], polygon[1], polygon[2]);
			triangles.push_back(triangle);
			for (int i=3;i<polygon.size();i++){
				TriangleString triangle2(polygon[i-1], polygon[i], polygon[0]);
				triangles.push_back(triangle2);
			}
		}
	}
	ifs.close();

	std::map<TriangleIndex,int> cache;
	for (int i=0;i<triangles.size();i++){
		TriangleString triangleString = triangles[i];
		for (int j=0;j<3;j++){
			TriangleIndex index = triangleString.get(j);
			std::map<TriangleIndex,int>::iterator cachedIndex = cache.find(index);
			if (cachedIndex != cache.end()) {
				outIndices.push_back(cachedIndex->second);
			} else {
				int vertexIndex = outPositions.size();
				outPositions.push_back(positions[index.position-1] * scale);
				if (index.normal != -1){
					outNormal.push_back(normals[index.normal-1]);
				}
				if (index.uv != -1) {
					outUv.push_back(uvs[index.uv-1]);
				}
				outIndices.push_back(vertexIndex);
				cache[index] = vertexIndex;
			}
		}
	}
	//cout <<"Indices "<< outIndices.size() << endl;
	//cout <<"Positions "<< outPositions.size() << endl;
	//printDebug(outPositions, outIndices);
	return true;
}

void printDebug(std::vector<point3> &positions, std::vector<int> &indices){
	for (int i=0;i<indices.size();i++){
		std::cout << positions[indices[i]] <<" ";
		if ((i+1)%3==0){
			std::cout << std::endl;
		}
	}
}
