#include "header.h"

int main() {

	findFileList("","");

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
			returnName = findFileList(path + str + "\\", str);

			allNameList.push_back(returnName);

			if (badNameCheck(returnName)) {//������ �̸��� ��
				badNameList.push_back(returnName);
			}

		}
		bResult = FindNextFile(hSearch, &wfd);
	} while (bResult);
	FindClose(hSearch);

	if (dir_count == 0 && image_count > 0) {//leaf node. ���ʿ� ���� ����.


		printf("find comic folder. zip %s\n",name.c_str());
		zipFolder(path, name);
	}

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


	//��� ���� �˻�
	hSearch = FindFirstFile(searchPath, &wfd);
	do {
		//����(�������� �ȿ� �߰�)
		sprintf(&targetPath[len], "%s", wfd.cFileName);
		ZipAdd(hz, wfd.cFileName, targetPath);
		DeleteFile(targetPath);
		bResult = FindNextFile(hSearch, &wfd);
	} while (bResult);
	FindClose(hSearch);

	//��������
	CloseZip(hz);

	//���� ����
	RemoveDirectory(dirPath);

}
