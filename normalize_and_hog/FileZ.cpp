#include "FileZ.h"

FileZ::FileZ()
{
	this->name = "";
	this->subdir = true;
}

FileZ::FileZ(string name, string type, bool subdir)
{
	this->name = name;
	this->type = type;
	this->subdir = subdir;
}

bool FileZ::isExist()
{
	if ((_access(this->name.c_str(), 0)) != -1)
	{
		return true;
		/* Check for write permission */
		/*if ((_access("ACCESS.C", 2)) != -1)
		printf("File ACCESS.C has write permission ");*/
	}
	else
		return false;

}

void FileZ::getFiles(string name)
{
	intptr_t   hFile = 0;
	struct _finddata_t fileinfo;
	string p;

	if ((hFile = _findfirst(p.assign(name).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR) && subdir)
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(name).append("\\").append(fileinfo.name));
			}
			else
			{
				string itype(fileinfo.name);
				itype = itype.substr(itype.find_first_of('.') + 1);
				if (this->type.empty() || !itype.compare(this->type))
				{
					Info info;
					info.directory = name;
					info.file = fileinfo.name;
					info.path = name + "\\" + fileinfo.name;
					info.name = info.file.substr(0, info.file.find_first_of("."));
					this->files.push_back(info);
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

void FileZ::getFiles()
{
	getFiles(this->name);
}

void FileZ::copyFile(string target)
{
	fstream f1(this->name, ios::in);
	fstream f2(target, ios::out);

	f2 << f1.rdbuf();

	f2.close();
	f1.close();
}

void FileZ::writeResult(string path, string flag)
{
	ofstream ofs;
	ofs.open(path);
	if (ofs.fail())
	{
		cout << "?" << endl;
	}
	if (flag == "f")
	{
		//for (auto it = this->subfile.begin(); it != this->subfile.end(); it++)
		//{
		//	ofs << *it << endl;
		//}
	}
	else if (flag == "p")
	{
		/*for (auto it = this->subpath.begin(); it != this->subpath.end(); it++)
		{
		ofs << *it << endl;
		}*/
	}


	ofs.close();
}

void FileZ::copyFile(string source, string target)
{
	fstream f1(source, ios::in);
	fstream f2(target, ios::out);

	f2 << f1.rdbuf();

	f2.close();
	f1.close();
}
