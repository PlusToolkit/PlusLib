/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
\file SingleWallCalibrationApp.cxx 
\brief Start an application to perform single wall calibration
*/ 

//#include <vtkSingleWallCalibration.h>
//#include <vtkSmartPointer.h>
/*#include "iostream"

int main( int argc, char** argv )
{
  // An example!
  //vtkSmartPointer<vtkSingleWallCalibration> aCalibrationObject = vtkSmartPointe<vtkSingleWallCalibration>::New();

  // Write your C++ here


	system("pause");
	return 0;
}*/

#include <iostream>
#include "vtkMatrix3x3.h"
#include "vtkSmartPointer.h"
#include "vtkMath.h"
#include "vnl/vnl_matrix.h"
#include "vnl/algo/vnl_qr.h"
#include "math.h"

using namespace std; 
#define PI 3.1415

struct Extract_output{

	double *nn;
	double *uu;
	double *vv;
	double k;

};


class SingleWall {

	double alpha,beta,lambda;
public:
	vtkMatrix3x3 * MyRotation (double [3]);
	double * Decompose (vtkMatrix3x3 *);
	double * matrix_multiplication(double *, double *, int, int , int , int);
	//Extract_output extract_x(double *);

	//void SingleWallMain ();

};

double norm (double a[3]){

	double s; 

	s = sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
	
	return s;
}

Extract_output extract_x(double * a){

	double x [18] = {0};
	int l=0;
	double nn[3][6] ={0}, uu[3][3]={0}, v1[3]={0}, vv[3][3]={0},k,f1[9]={0},f2[9]={0};
	double n1[3],nrm=0, u1[3]={0};
	
	int l1=0,l2=0;

	for (int i=0; i<18; i++){

		x[i] = a [i];
		
	}

	/////////////////
	
	for (int i =0 ; i<6; i++){

		l1= (i)*3;
		l2 =(i)*3+2;
		
		l=0;


		for (int j=l1; j<l2+1; j++){

			n1[l] = x[j];
			l=l+1;
			
		}

		nrm = norm(n1);

		if (n1[0] >0) {

			n1[0] = n1[0]/nrm;
			n1[1] = n1[1]/nrm;
			n1[2] = n1[2]/nrm;
			
		}
		else {
			
			n1[0] = -n1[0]/nrm;
			n1[1] = -n1[1]/nrm;
			n1[2] = -n1[2]/nrm;
			
		}

		for (int l=0; l<3; l++){

			nn[l][i] = n1[l];
		
		}
				
	}
	////////////// normal vector of the plane is finished //////////

	for (int i=0; i<3;i++){

		u1[0] = x[i];
		u1[1] = x[3+i];
		u1[2] = x[6+i];
		
		nrm = norm(u1);

		if (u1[0] >0) {

			u1[0] = u1[0]/nrm;
			u1[1] = u1[1]/nrm;
			u1[2] = u1[2]/nrm;
			
		}
		else {
			
			u1[0] = -u1[0]/nrm;
			u1[1] = -u1[1]/nrm;
			u1[2] = -u1[2]/nrm;
			
		}

		for (int l=0; l< 3 ; l++){

			uu[l][i] = u1[l];
		
		
		}

	}

	//////////////  u is finished ///////////////

	for (int i=0; i<3;i++){

		v1[0] = x[i+9];
		v1[1] = x[i+12];
		v1[2] = x[i+15];
		
		nrm = norm(v1);

		if (v1[0] >0) {

			v1[0] = v1[0]/nrm;
			v1[1] = v1[1]/nrm;
			v1[2] = v1[2]/nrm;
			
		}
		else {
			
			v1[0] = -v1[0]/nrm;
			v1[1] = -v1[1]/nrm;
			v1[2] = -v1[2]/nrm;
			
		}

		for (int l=0; l< 3 ; l++){

			vv[l][i] = v1[l];
		
		
		}

	}


	///////////////////// calculate k /////////////////////

	for (int i=0; i<9; i++){

		f1[i] = x[i];
		f2[i] = x[i+9];

	}

	double normf1=0, normf2=0;

	for (int i=0; i<9; i++){

		normf1 = normf1 + f1[i]*f1[i];
		normf2 = normf2 + f2[i]*f2[i];
	}

	normf1 = sqrt(normf1);
	normf2 = sqrt(normf2);

	k = (normf2)/(normf1);


	/////////////////////////////////////////////////////
	Extract_output f;

	f.nn = *nn;
	f.uu = *uu;
	f.vv = *vv;
	f.k = k;

	return f;

}


double * SingleWall::matrix_multiplication(double *p1, double *p2, int s11, int s12, int s21, int s22) {
	double *a, *b, *c;
	int i,j,k=0; 
	
	a = new double [s11*s12];
	b = new double [s21*s22];
	c = new double [s11*s22];

	// getting input arrays from their pointers

	for (int i=0; i<s11*s12; i++){
		a[i] = p1[i];
	}

	for (int i=0; i< s12*s22; i++){
		b[i] = p2[i];
	}




	/*k=0;
	for (i=0; i<s21; i++){
		for (j=0; j<s22; j++){
			
			b[i][j] = *(p2+k);
			k++;

		}
	}*/

//----------------------- multiplying of two  2D arrays------------------------------------------/

	double n1=0, n2=0;
	k=0;
	for (int i=0;i<s11;i++){
		for(int j=0;j<s22;j++){

			c[(i)*s11 + j] =0;
			
			for (int l=0; l<s12; l++){
				//n1 = (i)*s12 + j;
				//n2 = (j)*s22 + i;
				//cout << (i)*s11 + j <<"\t" << i*s12 + l << "\t" <<  l*s22 + j;
				c[(i)*s11 + j] = c[(i)*s11 + j] + a[i*s12 + l]*b[l*s22 + j];
			}

			
		}
	}

	delete [] a;
	delete [] b;
	return c;
}

double * SingleWall::Decompose(vtkMatrix3x3 *R){

	double tmp, a,e,r , rot[3] = {0};
	int k=0;

	
	tmp = -R->GetElement(2,0);
	
	if( tmp <= -1){
		e = -PI/2;}

	else if( tmp>= 1){
		e = PI/2;}
	
	else{ 
		e = asin(tmp);}

	////////////////////////////

	tmp = R->GetElement(0,0)/cos(e);
	
	if( tmp<=-1 ){
		a = PI;
	}
	else if ( tmp >=1 ) {
		a=0;
	}
	else {
		a = acos(tmp);
	}
			
	
	if ( (sin(a)*cos(e) > 0 ? 1:0 ) ^ (R->GetElement(1,0) >0 ? 1:0) ){\
	
		a = -a;

	}

	///////////////////////////

	tmp = R->GetElement(2,2)/cos(e);
	
	if( tmp <= -1){
		r = PI;
	}
	
	else if (tmp >= 1) {
		r = 0;
	}

	else{
		r = acos(tmp);
	}

	// write second xor here


	if ( (sin(r)*cos(e) > 0 ? 1:0 ) ^ (R->GetElement(2,1) >0 ? 1:0) ){
	
		r = -r;

	}


	////////////////////////


	a = a*180/PI; // alfa
	e = e*180/PI; // beta
	r = r*180/PI; // gamma
	
	rot [0] = a; rot [1] = e; rot [2] = r; 

	return rot;

}


vtkMatrix3x3 * SingleWall::MyRotation (double rot[3]){

	double Rotx[3][3],Roty[3][3],Rotz[3][3], elm[9]={0};
	
	vtkMatrix3x3 * Rotxyz;
	Rotxyz = vtkMatrix3x3::New();

	double	al, bet, gamma, c[3][3]={0}; 
	int i,j , k;

	// calculating the rotation matrices

	al = rot[0] * PI/180; 
	bet = rot[1] * PI/180; 
	gamma = rot[2] * PI/180; 

	Rotx[0][0] = 1; Rotx[0][1] = 0; Rotx[0][2] = 0; Rotx[1][0] = 0; Rotx[1][1] = cos(gamma); Rotx[1][2] = -sin(gamma); Rotx[2][0] = 0; Rotx[2][1] = sin(gamma); Rotx[2][2] = cos(gamma);

	Roty[0][0] = cos(bet); Roty[0][1] = 0; Roty[0][2] = sin(bet); Roty[1][0] = 0; Roty[1][1] = 1; Roty[1][2] = 0; Roty[2][0] = -sin(bet); Roty[2][1] = 0; Roty[2][2] = cos(bet);

	Rotz[0][0] = cos(al); Rotz[0][1] = -sin(al); Rotz[0][2] = 0; Rotz[1][0] = sin(al); Rotz[1][1] = cos(al); Rotz[1][2] = 0; Rotz[2][0] = 0; Rotz[2][1] = 0; Rotz[2][2] = 1;

		
	//---------------multiplying of rotation matrices----------------------------------

	vtkMath::Multiply3x3(Roty,Rotx,c);
	vtkMath::Multiply3x3(Rotz,c,c);

	k=0;
	for (int i=0; i<3; i++){
		for(int j=0; j<3; j++){
			
			elm[k] = c[i][j];
			k++;

		}
	}

	Rotxyz->DeepCopy(elm);

	
	// = matrix_multiplication(*Roty,*Rotx);
	 //= matrix_multiplication(*Rotz,*Rotxyz);
	
	return Rotxyz; // :)
}


int main(int, char *[])
{


	// --------- Reading File ----------------

	double Rp[100][3]={0};
	double Tp[100][3]={0};
	double m[100]={0}, x_center[100]={0}, y_center[100]={0};

	
	ifstream fin1("Rp1stCol.txt"); // another syntax : ifstream fin1; fin1.open("whatever.txt");
	ifstream fin2("Rp2ndCol.txt"); // http://www.cplusplus.com/reference/fstream/ifstream/
	ifstream fin3("Rp3rdCol.txt");

	ifstream fin4("Tp1stCol.txt");
	ifstream fin5("Tp2ndCol.txt");
	ifstream fin6("Tp3rdCol.txt");

	for (int i=0; i<100; i++){
	
		fin1 >> Rp[i][0]; fin4 >> Tp[i][0];
		fin2 >> Rp[i][1]; fin5 >> Tp[i][1];
		fin3 >> Rp[i][2]; fin6 >> Tp[i][2]; 

		
	}
	
	fin1.close();
	fin2.close();
	fin3.close();
	fin4.close();
	fin5.close();
	fin6.close();


//................ Reading x_center; y_center and "m=slope of line".........

	ifstream fin7 ("m.txt");
	ifstream fin8 ("x_center.txt");
	ifstream fin9 ("y_center.txt");

	for (int i=0; i<100; i++){

		fin7 >> m[i];
		fin8 >> x_center[i];
		fin9 >> y_center[i];
	
	}
	
	fin7.close();
	fin8.close();
	fin9.close();

	// ------------- Reading Finished ---------------

	
	SingleWall Calib1;
	vtkMatrix3x3 *aMatrix;
	vtkMath *p2;
	
	double RotateAnlgle [3];
	double Num = sizeof(m)/sizeof(double); // number of images acquired
	double R0[3][3], rot0[3]={0}; double *p;
	double R_Probe_to_Reference_i [3][3][100], RD_i[3][3][100],R0I[3][3], tmp[3][3];
	int k=0;
	
	rot0 [0] = Rp[0][0]; rot0[1] = Rp[0][1]; rot0[2] = Rp[0][2];
	aMatrix = Calib1.MyRotation(rot0);
	p = aMatrix->GetData();
	
	k=0;
	for (int l1=0; l1<3; l1++){
		for (int l2=0; l2<3; l2++){

			R0[l1][l2] = *(p+k); // rotation at the initial position
			k++;
		}
	}

	double t_PR_i[3][100]={0};

	for (int i=0; i<3; i++){
		for (int j=0; j<100; j++){

			t_PR_i[i][j] = Tp[j][i];
		}
	}
	
	//cout << t_PR_i[2][0] << "\t" << t_PR_i[2][99];


	
	for (int i=0; i<100; i++){

		RotateAnlgle[0] = Rp[i][0]; RotateAnlgle[1]=Rp[i][1]; RotateAnlgle[2] = Rp[i][2];
		aMatrix = Calib1.MyRotation(RotateAnlgle);

		p = aMatrix->GetData();
		
		k=0;
		for (int l1=0; l1<3; l1++){
			for (int l2=0; l2<3; l2++){

				R_Probe_to_Reference_i[l1][l2][i] = *(p+k);
				k++;
			}
		}
		
	}

	// writing Equ.2 of the paper
	
	for (int i=1; i<Num; i++){
		
		vtkMath::Invert3x3(R0,R0I);
		
		for (int l1=0; l1<3; l1++){
			for (int l2=0; l2<3; l2++){

				tmp[l1][l2] = R_Probe_to_Reference_i[l1][l2][i];
				
			}
		}
		
		double c[3][3];
		vtkMath::Multiply3x3(tmp,R0I,c);

		//cout << c[0][0];
		
		
		for (int l1=0; l1<3; l1++){
			for (int l2=0; l2<3; l2++){

				RD_i[l1][l2][i] = c[l1][l2];
				
			}
		}

	}

	double A[99][18]={0};
	double tmp2[18]={0};

	
	for (int i=1; i<Num; i++){
		
		k=0;
		for (int l1=0; l1<3; l1++){
			for (int l2=0; l2<3; l2++){
				
				tmp2[k] = RD_i[l2][l1][i];
				tmp2[k+9] = m[i]*RD_i[l2][l1][i];
				k++;

			}
		}

		for (int l1=0; l1<18; l1++){
			A[i-1][l1] = tmp2[l1];
		}

	}

	//------------ Now Solve for X ------------------

	double B2[99]={0}, A2[99][17]={0}, lambda=0.5;
	double A2T [17][99]={0};


	for (int i=0; i<100; i++){
		B2[i] = -A[i][0];
	}

	for (int l1=0; l1<99; l1++){
		for (int l2=0; l2<17; l2++){

			A2[l1][l2] = A[l1][l2+1];

		}
	}

	for (int i=0; i<17; i++){
		for (int j =0; j<99; j++){

			A2T[i][j] = A2[j][i];
		}
	}

	
	// multuplying a 17*99 matrix by 99*17 matrix
	p = Calib1.matrix_multiplication(*A2T, *A2, 17, 99, 99,17); // here use vnl class to multiply these two matrices

	
	vnl_matrix<double> tmp3(17, 17);
	k=0;


	for (int i=0; i<17; i++){
		for(int j=0; j<17; j++){
			
			tmp3[k/17][k-((k/17)*17)] = *(p+k);
			k++;
		}
	}

	double lambda_I[17][17]={0};

	for (int i=0; i<17; i++){
		for (int j=0; j<17; j++){
			if(i==j)
				lambda_I[i][j] = lambda;
			else
				lambda_I[i][j] = 0;
		}
	}

	for (int i=0; i<17; i++){
		for (int j=0; j<17; j++){
			tmp3[i][j] = tmp3[i][j] + lambda_I[i][j];
		}
	}

	//--- compute inverse of a matrix

	//vnl_matrix<double> matrix(17, 17);	

	vnl_qr<double> q(tmp3);
	vnl_matrix<double> inverse = q.inverse();  // inverse contains the inverse matrix.

	//p = inverse.data_block();
	inverse.copy_out(p);

	double INV[17][17]={0};
	k=0;
	for (int i=0; i<17; i++){
		for(int j=0; j<17; j++){

			INV[i][j] = *(p+k);
			k++;

		}
	} // INV is correct. tested!
 
	
	vnl_matrix<double> tmp4(17,99); // filled with A2T
	vnl_matrix<double> tmp5 (99,1); // filled with B2

	for (int i=0; i<17; i++){
		for (int j=0; j<99; j++){

			tmp4[i][j] = A2T[i][j];
		}
	}
	for(int i=0; i<99; i++){
		tmp5[i][0] = B2[i];
	}



	/*p = Calib1.matrix_multiplication(*A2T, B2, 17, 99, 99,1);


	double C[17]={0};

	//cout << p[0] << "\t" << p[1];


	k=0;
	for (int i=0; i<17; i++){
		C[i] = p[k];
		k++;
	}*/

	vnl_matrix<double> A2TB2 = tmp4*tmp5; // A2'*B2
	//p = A2TB2.data_block();

	A2TB2.copy_out(p);

	double C[17]={0};

	for (int i=0; i<17; i++){
		C[i] = *(p+i);
	} // A2TB2 tested, correct!

	vnl_matrix<double> tmp6 = inverse*A2TB2; // estimation of X
	
	//p = tmp6.data_block();

	tmp6.copy_out(p);

	double X2_est[17]={0};
	
	for (int i=0; i<17; i++){
		X2_est[i] = *(p+i);
	}



	double X_est0[18]={0};

	X_est0[0] = 1;

	for (int i=1; i<18; i++){

		X_est0[i] = X2_est[i-1];
	
	}


	double scale_est=0;

	for(int i=0; i<9; i++){

		scale_est = scale_est + pow(X_est0[i],2);

	}

	scale_est = sqrt(scale_est);
	scale_est = 1/scale_est;


	double X_est[18]={0};

	for (int i=0; i<18; i++){

		X_est[i] = X_est0[i]*scale_est;
	}

	double k_est=0;

	for(int i=9; i<18; i++){

		k_est = k_est + pow(X_est[i],2);

	}

	k_est = sqrt(k_est);
	
	Extract_output unv;

	unv = extract_x(X_est);
	double nn[3][6]={0}, uu[3][3]={0}, vv[3][3]={0},k_est2=0;

	//------- Builing the vectors ------------

	//builing the first matrix
	k=0;

	for (int i=0; i<3; i++){
		for (int j=0; j<6; j++){
			
			nn[i][j] = *(unv.nn + k);
			k++;

		}
	}
	
	///////////////  builing the second matrix
	k=0;

	for (int i=0; i<3; i++){
		for (int j=0; j<3; j++){
			
			uu[i][j] = *(unv.uu + k);
			k++;

		}
	}
	
	///////////////  builing the third matrix
	k=0;

	for (int i=0; i<3; i++){
		for (int j=0; j<3; j++){
			
			vv[i][j] = *(unv.vv + k);
			k++;

		}
	}
	
	k_est2 =unv.k; 

	double n_est[3]={0}, U0_est[3]={0}, V0_est[3]={0};

	for(int i=0; i<3; i++){
		n_est[i] = nn[i][0];
		U0_est[i]= uu[i][1];
		V0_est[i] = -vv[i][0];
	}

	double Z0_est[3]={0};
	vtkMath::Cross(U0_est,V0_est,Z0_est);
	
	double normCross = norm(Z0_est);

	for(int i=0; i<3; i++){
		Z0_est[i] = Z0_est[i] / normCross;
	}

	double R_Image_to_Reference_0_est [3][3]={0};

	for (int i=0; i<3; i++){
		R_Image_to_Reference_0_est[i][0] = U0_est[i];
		R_Image_to_Reference_0_est[i][1] = V0_est[i];
		R_Image_to_Reference_0_est[i][2] = Z0_est[i];
	}

	double R_Image_to_Probe_est[3][3]={0};

	vtkMath::Multiply3x3(R0I,R_Image_to_Reference_0_est,R_Image_to_Probe_est);

	
	double elm[9]={0};
	
	k=0;
	for (int i=0; i<3; i++){
		for(int j=0; j<3; j++){
			
			elm[k] = R_Image_to_Probe_est[i][j];
			k++;

		}
	}

	aMatrix->DeepCopy(elm);

	p = Calib1.Decompose(aMatrix);

	double r_IP_est [3]= {0};
	
	r_IP_est[0]=*p;
	r_IP_est[1]=*(p+1);
	r_IP_est[2]=*(p+2);

	cout << "Estimated Rotation" << "\n\n";
	cout << r_IP_est[0] <<"\t" << r_IP_est[1] << "\t" << r_IP_est[2] << "\n\n";



	//----------- Rotation Part is Finished. Start Translation ---------------------------

	double R_Image_to_Probe[3][3]={0};
	
	aMatrix = Calib1.MyRotation(r_IP_est);
	p = aMatrix->GetData();

	k=0;
	for(int i=0; i<3; i++){
		for(int j=0; j<3; j++){
			R_Image_to_Probe[i][j] = *(p+k);
			k++;
		}
	}
	
	double R_Image_to_Reference[3][3]={0}, Ui_est[3][100]={0}, Vi_est[3][100]={0} ; 
	double atemp[3][3]={0};

	for (int i=0; i<Num; i++){

		for (int l1=0; l1<3; l1++){
			for(int l2=0; l2<3; l2++){

				atemp[l1][l2] = R_Probe_to_Reference_i[l1][l2][i];

			}
		}

		vtkMath::Multiply3x3(atemp,R_Image_to_Probe,R_Image_to_Reference);

		 Ui_est[0][i] = R_Image_to_Reference[0][0] ; Ui_est[1][i] = R_Image_to_Reference[1][0]; Ui_est[2][i] = R_Image_to_Reference[2][0]; 
		 Vi_est[0][i] = R_Image_to_Reference[0][1] ; Vi_est[1][i] = R_Image_to_Reference[1][1]; Vi_est[2][i] = R_Image_to_Reference[2][1]; 
		
	}
	
	double ni[100][3]={0};
	for (int i=0; i<Num; i++){
		
		ni[i][0] = n_est[0]*R_Probe_to_Reference_i[0][0][i] + n_est[1]*R_Probe_to_Reference_i[1][0][i] + n_est[2]*R_Probe_to_Reference_i[2][0][i];
		ni[i][1] = n_est[0]*R_Probe_to_Reference_i[0][1][i] + n_est[1]*R_Probe_to_Reference_i[1][1][i] + n_est[2]*R_Probe_to_Reference_i[2][1][i];
		ni[i][2] = n_est[0]*R_Probe_to_Reference_i[0][2][i] + n_est[1]*R_Probe_to_Reference_i[1][2][i] + n_est[2]*R_Probe_to_Reference_i[2][2][i];
	}
	
	double Sx =1, Sy=1;

	double xi[100]={0}, yi[100]={0};

	for (int i=0; i<100; i++){
		xi[i] = x_center[i];
		yi[i] = y_center[i];
	}
	
	double Ti[3]={0}, Ui[3]={0}, Vi[3]={0};
	vnl_matrix<double> ci(100,1);
	for (int i=0; i<Num; i++){

		Ti[0] = Tp[i][0]; Ti[1] = Tp[i][1]; Ti[2] = Tp[i][2];
		Ui[0] = Ui_est[0][i]; Ui[1] = Ui_est[1][i]; Ui[2] = Ui_est[2][i]; 
		Vi[0] = Vi_est[0][i]; Vi[1] = Vi_est[1][i]; Vi[2] = Vi_est[2][i]; 

		ci[i][0] = vtkMath::Dot(n_est,Ti) + Sx*xi[i]*vtkMath::Dot(Ui,n_est) + Sy*yi[i]*vtkMath::Dot(Vi,n_est);
		

	}

	vnl_matrix<double> G(100,4);
	vnl_matrix<double> GT(4,100);


	for (int i=0; i<100; i++){
		for(int j=0; j<4; j++){

			G[i][j] = ni[i][j];


		}
		G[i][3] = -1;
	}

	GT = G.transpose();

	vnl_matrix<double> tmp7 = GT*G;

	vnl_qr<double> q2(tmp7);
	vnl_matrix<double> invGTG = q2.inverse();

	tmp7 = -invGTG*GT;
	
	tmp7 = tmp7*ci;

	tmp7.copy_out(p);

	double Est[4]={0};

	for (int i=0; i<4; i++){
		Est[i] = *(p+i);
	}
	
	double t_IP_est[3]={0};

	t_IP_est[0] = Est[0]; t_IP_est[1] = Est[1]; t_IP_est[2] = Est[2];

	double d_est = -Est[3];

	//---------- Translation Done!------ Calibration Finished ---------

	cout << "Estimated Translation: " << "\n\n"; 
	cout << t_IP_est[0] << "\t" << t_IP_est[1]<< "\t" << t_IP_est[2] << "\n\n";

	cout << "Estimated n:"<< "\n" << n_est[0] << "\t" << n_est[1] << "\t" << n_est[2] << "\n\n";

	cout << "Estimated d (parameter of the plane) :" << "\t" << d_est <<"\n\n";

	
	
	/*tmp7.copy_out(p);

	double tmp8[4][100]={0};
	k=0;
	for (int i=0; i<4; i++){
		for(int j=0; j<100; j++){

			tmp8[i][j] = *(p+k);
			k++;

		}
	} */

	system("pause");
 
	return EXIT_SUCCESS; /// ???? whay is this for??!!

	//double **p1;
	//double *p1;
	//p1 = inverse.data_array();
	
	//p1 = inverse.data_block();

	/*double tmp4[17][17]={0};   Just to check that the inverse is working correctly.

	k=0;
	for (int i=0; i<17; i++){
		for(int j=0; j<17; j++){

			tmp4[i][j] = *(p1+k);
			k++;
		}
	}*/
	/*for (int i=0; i<17; i++){
		k=0;
		for (int j=0; j<17; j++){
			tmp0[k] = tmp3[i][j];
			k++;
		}
		
		p1[i] = tmp0;
	}


	double *J[2], J0[2], J1[2];
    double *JI[2], JI0[2], JI1[2];
	//int test;
	
	J[0] = J0; J[1] = J1;
    JI[0] = JI0; JI[1] = JI1;

    J[0][0] = 4;
    J[1][0] = 5;
    J[0][1] = 6;
    J[1][1] = 7;

  // Compute inverse Jacobian
    vtkMath::InvertMatrix(J,JI,2);

	cout << JI[0][0] << "\t" <<JI[1][0]<< "\n";
	cout << JI[0][1] << "\t" << JI[1][1];	
	//test = vtkMath::InvertMatrix(q,e,2);*/

	//aMatrix = vtkMatrix3x3::New(); // defines an identity matrix

	//rot[0]= 30; rot[1]=45; rot[2]=60;

	//aMatrix = Calib1.MyRotation(rot);

	//cout <<*aMatrix;

	//p = Calib1.Decompose(aMatrix);

	//cout << p[0] << "\t" <<p[1] <<"\t" <<p[2];

}