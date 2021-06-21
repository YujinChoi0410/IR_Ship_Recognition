double *Projection[4][25]; //projection data�� ������ ���

void Print_Confusion_Matrix(int **Confuse, int nclass)
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CChildFrame *pChildFrame = (CChildFrame *)pMainFrame->MDIGetActive(NULL);
	CImageProcessingView *pView = (CImageProcessingView *)pChildFrame->GetActiveView();
	CDC *dc = pView->GetDC();

	int i, j, y0, tot, cor, stot;
	float correct, err;
	char msg[128];

	//"Confusion Matrix:"��� ���� text ����ϱ�
	y0 = 10;
	sprintf(msg, "Confusion Matrix:");
	dc->TextOut(10, y0, msg, strlen(msg));
	y0 += 28;

	stot = 0;
	for (i = 0; i < nclass; i++) {
		tot = 0;
		for (j = 0; j < nclass; j++) tot += Confuse[i][j]; //class�� ������ ����
		stot += tot; //��ü ������ ����
		correct = (float)Confuse[i][i] * 100. / (float)tot; //class�� ����Ȯ�� ����ϱ�
		sprintf(msg, "%6.2f%% : ", correct); //class�� ����Ȯ�� ����ϱ�
		dc->TextOut(10, y0, msg, strlen(msg));
		for (j = 0; j < nclass; j++) {
			sprintf(msg, "%3d ", Confuse[i][j]); //�ش� class�� �νĵ� ���� ����ϱ�
			dc->TextOut(100 + j * 55, y0, msg, strlen(msg));
		}
		y0 += 28; //matrix ���� ���� 28
	}

	cor = 0;
	for (i = 0; i < nclass; i++) cor += Confuse[i][i]; //��ü���� ������ ������ ���ϱ�
	err = (float)(stot - cor)*100. / (float)stot; //����Ȯ�� ����ϱ�
	sprintf(msg, "total error:   %d out of %d : %6.2f%%", (stot - cor), stot, err); //����Ȯ�� ����ϱ�
	dc->TextOut(10, y0, msg, strlen(msg));

	pView->ReleaseDC(dc);
}

////////////////////////

void Display_JPG_File(char *FileName, int x0, int y0)
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CChildFrame *pChildFrame = (CChildFrame *)pMainFrame->MDIGetActive(NULL);
	CImageProcessingView *pView = (CImageProcessingView *)pChildFrame->GetActiveView();
	CImageProcessingDoc* pDoc = pView->GetDocument();

	pDoc->m_Image[pDoc->m_nImageCnt].Image1D = ReadJpeg(FileName, &pDoc->m_Image[pDoc->m_nImageCnt].nW, &pDoc->m_Image[pDoc->m_nImageCnt].nH);
	pDoc->m_Image[pDoc->m_nImageCnt].nPosX = x0;
	pDoc->m_Image[pDoc->m_nImageCnt].nPosY = y0;
	pDoc->m_nImageCnt++;

	pView->Display_Image();

}

/////////////////////////////

void CHomework::OnClickedShip1()
{
	int i, k, w[4], h[4], x0, y0, Num, **Label, *Area, w2, h2, x, y, num;
	BYTE **img[4], **img2;
	char FN[128];
	RECT Rc;

	//c����̺꿡 �ִ� 4���� ship ������ �о ���� img[i]�� �о���̱�
	for (i = 0; i < 4; i++) {
		sprintf(FN, "c:\\Ship%01d.jpg", i);
		Display_JPG_File(FN, 0, 200);
		GetCurrentImageInfo(&w[i], &h[i], &x0, &y0); //�Է¿������� �б�
		img[i] = cmatrix(h[i], w[i]); //�Է¿��� �޸� �Ҵ��ϱ�
		GetCurrentImageGray(img[i]); //ȸ���� ���� �б�
	}

	//���� ship ������ display�� ó�� ��ġ
	y0 = 540;
	x0 = 0;
	num = 0;
	for (i = 0; i < 4; i++) { //�Է� ���� 4���� ���Ͽ� �ݺ�
		Label = imatrix(h[i], w[i]); //Label ���� �޸� �Ҵ��ϱ�
		Area = (int *)malloc(w[i] * h[i] * sizeof(int)); //area��(�׷�ȭ�� �ȼ� ����) ������ �޸� �Ҵ��ϱ�		
		Image_Labeling(img[i], w[i], h[i], Label, Area, &Num); //image labeling �����Ͽ� ��ü ���� �����ϱ�

		for (k = 1; k <= Num; k++) { //Num: �ִ� label ��
			if (Area[k] >= 200) { //���ִ� ȭ���� ������ 200�� �̻��̶��
				Find_Region(Label, w[i], h[i], k, &Rc); //������ ��ġ���� ����ϱ�
				w2 = Rc.right - Rc.left + 1; //������ ���α���
				h2 = Rc.bottom - Rc.top + 1; //������ ���α���
				img2 = cmatrix(h2, w2); //���� ���� �޸� �Ҵ��ϱ�
				for (y = 0; y < h2; y++) {
					for (x = 0; x < w2; x++) {
						if (Label[y + Rc.top][x + Rc.left] == k) img2[y][x] = 255; //Label���� k�϶� ��ü�����̹Ƿ� ���
						else img2[y][x] = 0; //��ü���� �ƴϸ� ��Ӱ�
					}
				}
				DisplayCimage2D(img2, w2, h2, x0, y0, TRUE); //���� ���� display�ϱ�
				x0 += w2 + 10; //���� ���� ���� ���� ������ 10
				free_cmatrix(img2, h2, w2); //���� ���� �޸� �Ҵ� �����ϱ�
			}
		}
		//���� img[i]�� ���� ���� ������ ���� �ٿ� display�ϱ� ���� ��ġ
		y0 += 50;
		x0 = 0;

		//Label �޸� �Ҵ� �����ϱ�
		free(Area);
		free_imatrix(Label, h[i], w[i]);
	}

	//�Է¿��� �޸��Ҵ� �����ϱ�
	for (i = 0; i < 4; i++) {
		free_cmatrix(img[i], h[i], w[i]);
	}
}

//////////////////////////

void Compute_Normalized_Projection(BYTE **Img, int W, int H, double *Projection, int ProjNum)
{
	//�Էµ����� Img: ���� ship����

	int x, y, i, x1, x2, num;
	double step;

	x1 = -1;
	//������ ��->��, ��->�Ʒ��� ��ĵ
	for (x = 0; x < W; x++) {
		for (y = 0; y < H; y++) {
			if (Img[y][x] > 128) { //ó������ ��ü ȭ�Ҹ� ������ ��
				x1 = x;	//x1�� ��ü�� x��ǥ�� �ּڰ�
				goto n1;
			}
		}
	}

n1:
	x2 = -1;
	//������ ��->��, ��->�Ʒ��� ��ĵ
	for (x = W - 1; x >= 0; x--) {
		for (y = 0; y < H; y++) {
			if (Img[y][x] > 128) { //��ü ȭ�Ҹ� ������ ��
				x2 = x; //x2�� ��ü�� x��ǥ�� �ִ�
				goto n2;
			}
		}
	}

n2:
	step = (double)(x2 - x1) / (ProjNum - 1); //sampling step size ���ϱ�

	for (i = 0; i < ProjNum; i++) {
		x = x1 + (int)(step*i); //i��° projection x��ǥ ��ġ
		num = 0;
		for (y = 0; y < H; y++) { //y�� �������� ��ĵ�ؼ� ��ü�� �ȼ� ���� ����
			if (Img[y][x] > 128) num++;
		}
		Projection[i] = num; //Projection data�� �����ϱ�
	}

	for (i = 0; i < ProjNum; i++) { //��ü�� ���α��̷� ������ normalization
		Projection[i] /= (x2 - x1 + 1);
	}
}

///////////////////////////

int Nearest_Neightbor_Classification(int i, int j, int ProjNum, int ClassNum)
{
	//�Էµ����� i: ClassNum, j: IamgeNum

	int t, k, result_class;
	double dif, dif_min; //dif:template ������� difference
						 //dif_min:difference�� �ּڰ�

	dif_min = 9999;
	for (t = 0; t < ClassNum; t++) { //template 0~3(�� class�� ù��° ������ projection)�� ���Ѵ�.
		dif = 0;
		for (k = 0; k < ProjNum; k++) { //template�� projection 15���� ���� ������ ��
			dif += abs(Projection[i][j][k] - Projection[t][0][k]);
		}

		//template����(class 0~3)���� difference�� �ּڰ� ���ϱ�
		if (dif < dif_min) {
			dif_min = dif;
			result_class = t; //class number �����ϱ�
		}
	}

	return result_class;
}

//////////////////////////////

void CHomework::OnClickedShip2()
{
	int i, j, k, w[4], h[4], x0, y0, Num, **Label, *Area, w2, h2, x, y, num;
	BYTE **img[4], **img2;
	char FN[128];
	RECT Rc;
	int ClassNum, ImageNum, ProjNum, Classification_result, **Confuse;
	double sum;

	ClassNum = 4; //class ��, 
	ImageNum = 25; //�� class�� ���� ������ ��
	ProjNum = 15; //projection �� 

	//4���� ������ �о ���� img[i]�� �о���̱�
	for (i = 0; i < 4; i++) {
		sprintf(FN, "c:\\Ship%01d.jpg", i);
		Display_JPG_File(FN, 0, 200);
		GetCurrentImageInfo(&w[i], &h[i], &x0, &y0); //�Է¿������� �б�
		img[i] = cmatrix(h[i], w[i]); //�Է¿��� �޸� �Ҵ��ϱ�
		GetCurrentImageGray(img[i]); //ȸ���� ���� �б�
	}

	//���� ������ display�� ó�� ��ġ
	y0 = 540;
	x0 = 0;
	num = 0;
	for (i = 0; i < 4; i++) {
		Label = imatrix(h[i], w[i]); //Label ���� �޸� �Ҵ��ϱ�
		Area = (int *)malloc(w[i] * h[i] * sizeof(int)); //area��(�׷�ȭ�� �ȼ� ����) ������ �޸� �Ҵ��ϱ�		
		Image_Labeling(img[i], w[i], h[i], Label, Area, &Num); //image labeling �����Ͽ� ��ü ���� �����ϱ�

		num = 0;
		for (k = 1; k <= Num; k++) { //Num: �ִ� label ��
			if (Area[k] >= 200) { //���ִ� ȭ���� ������ 200�� �̻��̶��
				Find_Region(Label, w[i], h[i], k, &Rc); //������ ��ġ���� ����ϱ�
				w2 = Rc.right - Rc.left + 1; //������ ���α���
				h2 = Rc.bottom - Rc.top + 1; //������ ���α���
				img2 = cmatrix(h2, w2); //���� ���� �޸� �Ҵ��ϱ�
				for (y = 0; y < h2; y++) {
					for (x = 0; x < w2; x++) {
						if (Label[y + Rc.top][x + Rc.left] == k) img2[y][x] = 255; //Label���� k�϶� ��ü�����̹Ƿ� ���
						else img2[y][x] = 0; //��ü���� �ƴϸ� ��Ӱ�
					}
				}
				DisplayCimage2D(img2, w2, h2, x0, y0, TRUE); //���� ���� display�ϱ�
				Projection[i][num] = new double[ProjNum]; //projection �� ��ŭ ���� �Ҵ��ϱ�
				Compute_Normalized_Projection(img2, w2, h2, Projection[i][num], ProjNum); //normalized projection ����ϱ�
				x0 += w2 + 10; //���� ���� ���� ���ΰ��� 10
				free_cmatrix(img2, h2, w2); //���� ���� �޸� �Ҵ� �����ϱ�
				num++; //class �� ���� ��ü��
			}
		}
		//���� img[i]�� ���� ���� ������ ���� �ٿ� display�ϱ� ���� ��ġ
		y0 += 50;
		x0 = 0;

		//Label ���� �޸� �Ҵ� �����ϱ�
		free(Area);
		free_imatrix(Label, h[i], w[i]);
	}

	Confuse = imatrix(ClassNum, ClassNum); //confusion matrix ���� �޸� �Ҵ��ϱ�

	for (i = 0; i < ClassNum; i++) { //confusion matrix �ʱ�ȭ
		for (j = 0; j < ClassNum; j++) {
			Confuse[i][j] = 0;
		}
	}

	for (i = 0; i < ClassNum; i++) { //4���� class �߿� ��� class�� ���� ������� ����ϱ�
		for (j = 1; j < ImageNum; j++) {
			Classification_result = Nearest_Neightbor_Classification(i, j, ProjNum, ClassNum);
			Confuse[i][Classification_result]++; //confusion matrix�� �νĵ� ��� ī��Ʈ�ؼ� �����ϱ�

		}
	}

	Print_Confusion_Matrix(Confuse, ClassNum); //confusion matrix display�ϱ�

	//�޸� �Ҵ� �����ϱ�
	for (i = 0; i < ClassNum; i++) {
		for (j = 0; j < ImageNum; j++) {
			delete[] Projection[i][j];
		}
	}

	free_imatrix(Confuse, ClassNum, ClassNum);

	for (i = 0; i < 4; i++) {
		free_cmatrix(img[i], h[i], w[i]);
	}
}

