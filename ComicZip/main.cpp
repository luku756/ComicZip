#include "header.h"

#define MODE_ALL 1
#define MODE_SPECIFIC 2

//�巡��-��� ���� ����� ��� ��� ������ Ž��(MODE_ALL)
//�巡��-������� Ư�� ������ Ž���� ���(MODE_SPECIFIC)
int mode = MODE_ALL;

int main(int argc, char* argv[]) {

	if (argc == 1) {//�߰� ���� �巡��-��� ����
		printf("run for here.\n");
		findFileList("", "");
	}
	else {//���� �巡��-��� �� �ش� ���� ������ Ž��. �ǵ帮�� �� �Ǵ� ����(�ӽ����� ��)�� �������� �� ���ϴ� ������ ����.
		mode = MODE_SPECIFIC;
		for (int i = 1; i < argc; i++) {
			printf("run for there.\n%s\n",argv[i]);
			string path = argv[i];
			findFileList(path + "\\", "");//������ �ƴ� ��� �ش� ��ġ�� ������ ������ ���õ�����, ������ ��� Ž���ȴ�.
			findBadName(path + "\\", "");
		}
	}

}

//������ �̸��� ���ɼ� (-2ȭ , .5ȭ) Ȯ��
bool badNameCheck(string filename) {

	int sub = filename.find("-2");

	if (sub == -1) {
		sub = filename.find(".5");
	}

	//printf("filename : %s, sub : %d\n",filename.c_str(),sub);

	if (sub == -1)//������ �̸�
		return false;
	else//������ �̸�
		return true;
}

//������ �̸��� ���ɼ��� Ȯ��(7-2ȭ, 52.5ȭ ��)�ϰ�, ������ (7-1ȭ ��� 7ȭ, 52.0ȭ ��� 52ȭ ��) ���� Ȯ���ϰ� �̸��� �ٲ۴�.
void nameChange(vector<string> allNameList, vector<string> badNameList, string path) {

	string frontName; //������ �̸�
	string newName;	//����� ����� �̸�
	int sub;
	int type = 0;	//-2���� .5����

	for (int i = 0; i < badNameList.size(); i++) {
		sub = badNameList[i].find("-2");
		type = 1;

		if (sub == -1) {
			sub = badNameList[i].find(".5");
			type = 2;
		}

		//printf("bad name %s \n", badNameList[i].c_str());
		frontName = badNameList[i].substr(0, sub) + badNameList[i].substr(sub + 2);	//�� �̸��� �ִٸ� �����ؾ� ��
		newName = badNameList[i].substr(0, sub);

		if (type == 1)
			newName += "-1";
		else
			newName += ".0";

		newName += badNameList[i].substr(sub + 2);
		for (int j = 0; j < allNameList.size(); j++) {		

			if (allNameList[j] == frontName) {//������ �̸��� �ִ��� Ȯ��
				//printf("-------------bad name %s -> %s \n", allNameList[j].c_str(), newName.c_str());
				sub = rename((path + allNameList[j]+".zip").c_str(), (path + newName + ".zip").c_str());
			}

		}


	}

}

//���� �տ� ���� �۰����� ����. [~~]
//�۰����� �ƴѵ� �տ� �ٴ� ���� ���ٰ� ����.
void removeArtistName(string path, string& name) {
	int end = name.find("]");
	if (end > 0) {
		int st = name.find("[");
		if (st == 0) {
			string st = name.substr(end + 2);
			printf("Rename Derectory : %s -> %s\n", name.c_str(), st.c_str());
			string s1 = path + name;
			string s2 = path + st;

			int s = rename((path + name).c_str(), (path + st).c_str());
			name = st;
		}
	}
}

//path : ���, name : ������(��ȭ ���� �̸� - xxx24ȭ ��)
string findFileList(string path, string name) {

	string returnName;
	vector<string> allNameList;	//��ü ���� �̸� ���.
	vector<string> badNameList;	//������ ���� �̸� ���.

	HANDLE hSearch;
	WIN32_FIND_DATA wfd;
	bool bResult = true;

	int dir_count = 0; //�� ���� �ȿ� ���丮�� �� �� �ִ���. 
	int image_count = 0;	//�� ���� �ȿ� �̹��� ������ �� �� �ִ���. �̹����� ���� ������ leaf �� ���� X.

	//��� ���� �˻�
	hSearch = FindFirstFile((path + "*").c_str(), &wfd);	//���� ��ο�������.
	do {
		string str = string(wfd.cFileName);
		string format;	//Ȯ���� ����
		int index = str.find_last_of(".");
		if (index > 0) {	//������ �Ϻ� ��� Ȯ���� ����
			format = str.substr(str.find_last_of(".") + 1);
			transform(format.begin(), format.end(), format.begin(), tolower);//�ҹ���ȭ. ���ϸ��� .JPEG �� ��츦 ���


			if (format == "jpg" || format == "gif" || format == "jpeg" || format == "png" || format == "bmp" || format == "tif" || format == "tiff") {
				image_count++;
			}
		}

		if (wfd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && (strcmp(".", wfd.cFileName) != 0) && (strcmp("..", wfd.cFileName) != 0)) {//������. ���糪 ���� ������ X. ������ ��ͷ� ����
			dir_count++;
			removeArtistName(path,str);
			returnName = findFileList(path + str + "\\", str);

			if (mode == MODE_ALL) {//MODE_SPECIFIC �� ���, �Ŀ� ���� ������ �̸��� Ž���Ѵ�. �ߺ� Ž�� ����
				allNameList.push_back(returnName);

				if (badNameCheck(returnName)) {//������ �̸��� ��
					badNameList.push_back(returnName);
				}
			}
		}
		bResult = FindNextFile(hSearch, &wfd);
	} while (bResult);
	FindClose(hSearch);

	if (dir_count == 0 && image_count > 0) {//leaf node. ���ʿ� ���� ����.
		printf("find comic folder. zip %s\n",name.c_str());
		
		zipFolder(path, name);
	}

	if (mode == MODE_ALL) {
		nameChange(allNameList, badNameList, path);	//������ �̸� (-2ȭ , .5ȭ) ����
	}
	return name;
}

//�̹� ������ �����ϴ� ������ ����, ������ �̸��� ���� ��� ã�Ƽ� �����Ѵ�.
string findBadName(string path, string name) {

	string returnName;
	vector<string> allNameList;	//��ü ���� �̸� ���.
	vector<string> badNameList;	//������ ���� �̸� ���.

	HANDLE hSearch;
	WIN32_FIND_DATA wfd;
	bool bResult = true;

	//��� ���� �˻�
	hSearch = FindFirstFile((path + "*").c_str(), &wfd);	//���� ��ο�������.
	do {
		string str = string(wfd.cFileName);
		string format;	//Ȯ���� ����
		int index = str.find_last_of(".");
		if (index > 0) {	//������ �Ϻ� ��� Ȯ���� ����
			format = str.substr(str.find_last_of(".") + 1);
			transform(format.begin(), format.end(), format.begin(), tolower);//�ҹ���ȭ. ���ϸ��� .ZIP �� ��츦 ���
			
			if (format == "zip") {
				string filename = str.substr(0, str.size() - 4);	//.zip�� �� �̸�
				allNameList.push_back(filename);

				if (badNameCheck(filename)) {//������ �̸��� ��
					badNameList.push_back(filename);
				}
			}
		}

		if (wfd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && (strcmp(".", wfd.cFileName) != 0) && (strcmp("..", wfd.cFileName) != 0)) {//������. ���糪 ���� ������ X. ������ ��ͷ� ����
			findBadName(path + str + "\\", str);
		}
		bResult = FindNextFile(hSearch, &wfd);
	} while (bResult);
	FindClose(hSearch);
	
	nameChange(allNameList, badNameList, path);	//������ �̸� (-2ȭ , .5ȭ) ����

	return name;
}



void zipFolder(string path, string name) {

	char searchPath[500], zipPath[500], dirPath[500], targetPath[500], filePath[500];

	HANDLE hSearch;
	WIN32_FIND_DATA wfd;
	HZIP hz;
	bool bResult = true;
	int len;


	sprintf(searchPath, "%s*", path.c_str());
	sprintf(targetPath, "%s", path.c_str());
	path = path.substr(0, path.length() - 1);	//\\ ����
	sprintf(dirPath, "%s", path.c_str());
	sprintf(zipPath, "%s.zip", path.c_str(), name.c_str());

	len = strlen(targetPath);

	//zip����
	hz = CreateZip(zipPath, 0);

	int flag_last = 0;
	//��� ���� �˻�
	hSearch = FindFirstFile(searchPath, &wfd);
	do {
		if (flag_last != 0) {
			ZipAdd(hz, wfd.cFileName, targetPath);
		}
		flag_last = 0;
		//����(�������� �ȿ� �߰�)
		sprintf(&targetPath[len], "%s", wfd.cFileName);

		int fsize = (wfd.nFileSizeHigh * (MAXDWORD + 1)) + wfd.nFileSizeLow;
		if (fsize != 16293) {
			ZipAdd(hz, wfd.cFileName, targetPath);
			flag_last = 1;
		}
		DeleteFile(targetPath);
		bResult = FindNextFile(hSearch, &wfd);
	} while (bResult);



	FindClose(hSearch);

	//��������
	CloseZip(hz);

	//���� ����
	RemoveDirectory(dirPath);

}
