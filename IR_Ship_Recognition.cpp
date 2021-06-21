double *Projection[4][25]; //projection data를 저장할 행렬

void Print_Confusion_Matrix(int **Confuse, int nclass)
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CChildFrame *pChildFrame = (CChildFrame *)pMainFrame->MDIGetActive(NULL);
	CImageProcessingView *pView = (CImageProcessingView *)pChildFrame->GetActiveView();
	CDC *dc = pView->GetDC();

	int i, j, y0, tot, cor, stot;
	float correct, err;
	char msg[128];

	//"Confusion Matrix:"라는 제목 text 출력하기
	y0 = 10;
	sprintf(msg, "Confusion Matrix:");
	dc->TextOut(10, y0, msg, strlen(msg));
	y0 += 28;

	stot = 0;
	for (i = 0; i < nclass; i++) {
		tot = 0;
		for (j = 0; j < nclass; j++) tot += Confuse[i][j]; //class별 데이터 개수
		stot += tot; //전체 데이터 개수
		correct = (float)Confuse[i][i] * 100. / (float)tot; //class별 정답확률 계산하기
		sprintf(msg, "%6.2f%% : ", correct); //class별 정답확률 출력하기
		dc->TextOut(10, y0, msg, strlen(msg));
		for (j = 0; j < nclass; j++) {
			sprintf(msg, "%3d ", Confuse[i][j]); //해당 class로 인식된 개수 출력하기
			dc->TextOut(100 + j * 55, y0, msg, strlen(msg));
		}
		y0 += 28; //matrix 세로 간격 28
	}

	cor = 0;
	for (i = 0; i < nclass; i++) cor += Confuse[i][i]; //전체에서 정답인 개수만 더하기
	err = (float)(stot - cor)*100. / (float)stot; //오류확률 계산하기
	sprintf(msg, "total error:   %d out of %d : %6.2f%%", (stot - cor), stot, err); //오류확률 출력하기
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

	//c드라이브에 있는 4개의 ship 영상을 읽어서 각각 img[i]에 읽어들이기
	for (i = 0; i < 4; i++) {
		sprintf(FN, "c:\\Ship%01d.jpg", i);
		Display_JPG_File(FN, 0, 200);
		GetCurrentImageInfo(&w[i], &h[i], &x0, &y0); //입력영상정보 읽기
		img[i] = cmatrix(h[i], w[i]); //입력영상 메모리 할당하기
		GetCurrentImageGray(img[i]); //회색조 영상 읽기
	}

	//작은 ship 영상을 display할 처음 위치
	y0 = 540;
	x0 = 0;
	num = 0;
	for (i = 0; i < 4; i++) { //입력 영상 4개에 대하여 반복
		Label = imatrix(h[i], w[i]); //Label 영상 메모리 할당하기
		Area = (int *)malloc(w[i] * h[i] * sizeof(int)); //area값(그룹화된 픽셀 개수) 저장할 메모리 할당하기		
		Image_Labeling(img[i], w[i], h[i], Label, Area, &Num); //image labeling 진행하여 물체 형상 추출하기

		for (k = 1; k <= Num; k++) { //Num: 최대 label 값
			if (Area[k] >= 200) { //모여있는 화소의 개수가 200개 이상이라면
				Find_Region(Label, w[i], h[i], k, &Rc); //형상의 위치정보 계산하기
				w2 = Rc.right - Rc.left + 1; //형상의 가로길이
				h2 = Rc.bottom - Rc.top + 1; //형상의 세로길이
				img2 = cmatrix(h2, w2); //작은 영상 메모리 할당하기
				for (y = 0; y < h2; y++) {
					for (x = 0; x < w2; x++) {
						if (Label[y + Rc.top][x + Rc.left] == k) img2[y][x] = 255; //Label값이 k일때 물체지점이므로 밝게
						else img2[y][x] = 0; //물체지점 아니면 어둡게
					}
				}
				DisplayCimage2D(img2, w2, h2, x0, y0, TRUE); //작은 영상 display하기
				x0 += w2 + 10; //작은 영상 간의 가로 간격은 10
				free_cmatrix(img2, h2, w2); //작은 영상 메모리 할당 해제하기
			}
		}
		//다음 img[i]에 대한 작은 영상을 다음 줄에 display하기 위한 위치
		y0 += 50;
		x0 = 0;

		//Label 메모리 할당 해제하기
		free(Area);
		free_imatrix(Label, h[i], w[i]);
	}

	//입력영상 메모리할당 해제하기
	for (i = 0; i < 4; i++) {
		free_cmatrix(img[i], h[i], w[i]);
	}
}

//////////////////////////

void Compute_Normalized_Projection(BYTE **Img, int W, int H, double *Projection, int ProjNum)
{
	//입력데이터 Img: 작은 ship영상

	int x, y, i, x1, x2, num;
	double step;

	x1 = -1;
	//영상을 좌->우, 위->아래로 스캔
	for (x = 0; x < W; x++) {
		for (y = 0; y < H; y++) {
			if (Img[y][x] > 128) { //처음으로 물체 화소를 만났을 때
				x1 = x;	//x1은 물체의 x좌표의 최솟값
				goto n1;
			}
		}
	}

n1:
	x2 = -1;
	//영상을 우->좌, 위->아래로 스캔
	for (x = W - 1; x >= 0; x--) {
		for (y = 0; y < H; y++) {
			if (Img[y][x] > 128) { //물체 화소를 만났을 때
				x2 = x; //x2는 물체의 x좌표의 최댓값
				goto n2;
			}
		}
	}

n2:
	step = (double)(x2 - x1) / (ProjNum - 1); //sampling step size 구하기

	for (i = 0; i < ProjNum; i++) {
		x = x1 + (int)(step*i); //i번째 projection x좌표 위치
		num = 0;
		for (y = 0; y < H; y++) { //y축 방향으로 스캔해서 물체의 픽셀 개수 세기
			if (Img[y][x] > 128) num++;
		}
		Projection[i] = num; //Projection data로 대입하기
	}

	for (i = 0; i < ProjNum; i++) { //물체의 가로길이로 나눠서 normalization
		Projection[i] /= (x2 - x1 + 1);
	}
}

///////////////////////////

int Nearest_Neightbor_Classification(int i, int j, int ProjNum, int ClassNum)
{
	//입력데이터 i: ClassNum, j: IamgeNum

	int t, k, result_class;
	double dif, dif_min; //dif:template 영상과의 difference
						 //dif_min:difference의 최솟값

	dif_min = 9999;
	for (t = 0; t < ClassNum; t++) { //template 0~3(각 class의 첫번째 영상의 projection)과 비교한다.
		dif = 0;
		for (k = 0; k < ProjNum; k++) { //template과 projection 15개의 차의 절댓값의 합
			dif += abs(Projection[i][j][k] - Projection[t][0][k]);
		}

		//template영상(class 0~3)과의 difference의 최솟값 구하기
		if (dif < dif_min) {
			dif_min = dif;
			result_class = t; //class number 결정하기
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

	ClassNum = 4; //class 수, 
	ImageNum = 25; //각 class에 속한 영상의 수
	ProjNum = 15; //projection 수 

	//4개의 영상을 읽어서 각각 img[i]에 읽어들이기
	for (i = 0; i < 4; i++) {
		sprintf(FN, "c:\\Ship%01d.jpg", i);
		Display_JPG_File(FN, 0, 200);
		GetCurrentImageInfo(&w[i], &h[i], &x0, &y0); //입력영상정보 읽기
		img[i] = cmatrix(h[i], w[i]); //입력영상 메모리 할당하기
		GetCurrentImageGray(img[i]); //회색조 영상 읽기
	}

	//작은 영상을 display할 처음 위치
	y0 = 540;
	x0 = 0;
	num = 0;
	for (i = 0; i < 4; i++) {
		Label = imatrix(h[i], w[i]); //Label 영상 메모리 할당하기
		Area = (int *)malloc(w[i] * h[i] * sizeof(int)); //area값(그룹화된 픽셀 개수) 저장할 메모리 할당하기		
		Image_Labeling(img[i], w[i], h[i], Label, Area, &Num); //image labeling 진행하여 물체 형상 추출하기

		num = 0;
		for (k = 1; k <= Num; k++) { //Num: 최대 label 값
			if (Area[k] >= 200) { //모여있는 화소의 개수가 200개 이상이라면
				Find_Region(Label, w[i], h[i], k, &Rc); //형상의 위치정보 계산하기
				w2 = Rc.right - Rc.left + 1; //형상의 가로길이
				h2 = Rc.bottom - Rc.top + 1; //형상의 세로길이
				img2 = cmatrix(h2, w2); //작은 영상 메모리 할당하기
				for (y = 0; y < h2; y++) {
					for (x = 0; x < w2; x++) {
						if (Label[y + Rc.top][x + Rc.left] == k) img2[y][x] = 255; //Label값이 k일때 물체지점이므로 밝게
						else img2[y][x] = 0; //물체지점 아니면 어둡게
					}
				}
				DisplayCimage2D(img2, w2, h2, x0, y0, TRUE); //작은 영상 display하기
				Projection[i][num] = new double[ProjNum]; //projection 수 만큼 공간 할당하기
				Compute_Normalized_Projection(img2, w2, h2, Projection[i][num], ProjNum); //normalized projection 계산하기
				x0 += w2 + 10; //작은 영상 간의 가로간격 10
				free_cmatrix(img2, h2, w2); //작은 영상 메모리 할당 해제하기
				num++; //class 내 다음 물체로
			}
		}
		//다음 img[i]에 대한 작은 영상은 다음 줄에 display하기 위한 위치
		y0 += 50;
		x0 = 0;

		//Label 영상 메모리 할당 해제하기
		free(Area);
		free_imatrix(Label, h[i], w[i]);
	}

	Confuse = imatrix(ClassNum, ClassNum); //confusion matrix 공간 메모리 할당하기

	for (i = 0; i < ClassNum; i++) { //confusion matrix 초기화
		for (j = 0; j < ClassNum; j++) {
			Confuse[i][j] = 0;
		}
	}

	for (i = 0; i < ClassNum; i++) { //4개의 class 중에 어느 class와 가장 비슷한지 계산하기
		for (j = 1; j < ImageNum; j++) {
			Classification_result = Nearest_Neightbor_Classification(i, j, ProjNum, ClassNum);
			Confuse[i][Classification_result]++; //confusion matrix에 인식된 결과 카운트해서 대입하기

		}
	}

	Print_Confusion_Matrix(Confuse, ClassNum); //confusion matrix display하기

	//메모리 할당 해제하기
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

