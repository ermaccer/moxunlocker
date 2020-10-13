// moxunlocker.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <filesystem>

struct MoxHeader {
	int     header; //MOX!
	bool    bIsLocked;
	char    pad;
	short   version; // ?
	int     tris;
	int     faces;
	int     unk[4]; // seems to have materials number
};

struct tris {
	float x, y, z;
	float unk[7]; // normals uv and ?
};

struct face_locked {
	int faces[3];
};

struct face {
	short faces[3];
};

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		std::cout << "moxunlocker - Unlock MOX files from World Racing 2 by ermaccer\n"
			<< "Usage: moxunlocker <input>\n";
		return 1;
	}

	std::ifstream pFile(argv[1], std::ifstream::binary);

	if (!pFile) {
		std::cout << "ERROR: Could not open " << argv[1] << "!" << std::endl;
		return 1;
	}

	if (pFile)
	{
		MoxHeader header;

		pFile.read((char*)&header, sizeof(MoxHeader));

		if (header.header != 'MOX!') {
			std::cout << "ERROR: " << argv[1] << " is not a MOX file!" << std::endl;
			return 1;
		}

		if (!header.bIsLocked) {
			std::cout << "INFO: " << argv[1] << " is not locked!" << std::endl;
			return 1;
		}

		std::vector<tris> vTris;
		std::vector<face_locked> vFaces;
		for (int i = 0; i < header.tris; i++)
		{
			tris tr;
			pFile.read((char*)&tr, sizeof(tris));
			vTris.push_back(tr);
		}

		for (int i = 0; i < header.faces; i++)
		{
			face_locked f;
			pFile.read((char*)&f, sizeof(face_locked));
			vFaces.push_back(f);
		}


		std::string outName = "UNLOCKED_";
		outName += argv[1];
	
		std::ofstream oFile(outName, std::ofstream::binary);

		header.bIsLocked = false;

		oFile.write((char*)&header, sizeof(header));
		for (int i = 0; i < vTris.size(); i++)
			oFile.write((char*)&vTris[i], sizeof(tris));

		for (int i = 0; i < vFaces.size(); i++)
		{
			short f[3];
			f[0] = vFaces[i].faces[0];
			f[1] = vFaces[i].faces[1];
			f[2] = vFaces[i].faces[2];
			oFile.write((char*)&f, sizeof(f));
		}

		unsigned int size = sizeof(MoxHeader) + vTris.size() * sizeof(tris) + vFaces.size() * sizeof(face_locked);
		int copysize = std::filesystem::file_size(argv[1]) - size;
		std::unique_ptr<char[]> data = std::make_unique<char[]>(copysize);
		pFile.read(data.get(), copysize);
		oFile.write(data.get(), copysize);

		std::cout << "Saved to " << outName.c_str() << "!" << std::endl;
		std::cout << "Finished." << std::endl;
		
	}
}
