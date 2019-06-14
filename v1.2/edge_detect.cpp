/*
Edge Detector
@author: Het Thakkar
*/
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <cstdio>
#include <gdal.h>
#include <gdal_priv.h>
#include <bits/stdc++.h>
#include <ctime>
#include <fstream>
#include <iomanip>

using namespace std;

#define ImgType float

int main(int argc, char *argv[])
{

    if(!(argc == 5)){

        printf("USAGE: ./edge_detect fileName windowSize outputFilename n_processors\n");
        exit(0);

    }

    int window_size = atoi(argv[2]);
    int p = atoi(argv[4]);

    GDALDataset  *poDataset;
    GDALAllRegister();
 
    poDataset = (GDALDataset *) GDALOpen( argv[1], GA_ReadOnly );

    //Show file information
    printf( "Driver: %s/%s\n",
        poDataset->GetDriver()->GetDescription(),
        poDataset->GetDriver()->GetMetadataItem( GDAL_DMD_LONGNAME ) );

    printf( "Size is %dx%dx%d\n",
        poDataset->GetRasterXSize(), poDataset->GetRasterYSize(),
        poDataset->GetRasterCount() );

    int i,j,k;
    ImgType ***inputImg;
   

    int X_dim, Y_dim, Z_dim;

    X_dim = poDataset->GetRasterXSize();
    Y_dim = poDataset->GetRasterYSize();
    Z_dim = poDataset->GetRasterCount();

    ImgType *outputImg[Y_dim];
    // X_dim = 50;
    // Y_dim = 50;
    // Z_dim = poDataset->GetRasterCount();
    
    //Read input image
    GDALRasterBand  *poBand;

    inputImg =(ImgType ***) CPLMalloc(Z_dim * sizeof(ImgType **));

    for (i = 0; i < Z_dim; i++)
    {
        poBand = poDataset->GetRasterBand(i + 1);
        inputImg[i] = (ImgType **)CPLMalloc(Y_dim * sizeof(ImgType *));
        for (j = 0; j < Y_dim; j++)
        {
                inputImg[i][j] = (ImgType *)CPLMalloc(X_dim * sizeof(ImgType));
                poBand->RasterIO( GF_Read, 0, j, X_dim, 1, inputImg[i][j], X_dim, 1, GDT_Float32, 0, 0 );
        }
        
    }

    //Allocate output image memory
    for (j = 0; j < Y_dim; j++)
    {
        outputImg[j] = (ImgType *)malloc(X_dim * sizeof(ImgType));
    }

    int imin, imax, jmin, jmax, ti, tj ,tk;
    float distfrac[4*(window_size)*(window_size)], tempsum;
    int n_elements;
    float fraction = 2.0;
    //Progress bar variables
    float barWidth, pos, localCount, threadShare;
    int tid;
    float progress;
    barWidth = 70;
    progress = 0.0;
    localCount = 0;

    double start_time = omp_get_wtime();

    
    if( p == -1 ){
        p = omp_get_max_threads();
    }

    //Set p threads for execution
    omp_set_num_threads(p);
    threadShare = Y_dim/p;

    printf("Starting computation with %d threads\n\n",p, threadShare);

    #pragma omp parallel for shared(inputImg, window_size, fraction, X_dim, Y_dim, Z_dim, outputImg, threadShare, barWidth) private(ti, tj, tk, i, j, k, tempsum, distfrac, n_elements, imax, imin, jmax, jmin, localCount, progress, pos, tid) collapse(2)
    for (j = 0; j < Y_dim; j++){
        // printf("j : %d\n",j);
        for (i = 0; i < X_dim; i++){
           
            // printf("i : %d\n", i);

            if(i == 0){

                localCount += 1;
                tid = omp_get_thread_num();
                if(tid == 0){
                                                  
                    progress = (float)(localCount/threadShare);
                    std::cout << "[";
                    int pos = barWidth * progress;
                    for (int i = 0; i < barWidth; ++i) {
                        if (i < pos) std::cout << "=";
                        else if (i == pos) std::cout << ">";
                        else std::cout << " ";
                    }
                    std::cout << "] " << int(progress * 100.0) << " %\r";
                    std::cout.flush();
                    
                }


            }

            imin = (i - window_size) > 0 ? (i - window_size) : 0;
            imax = (i + window_size) > X_dim ? X_dim : (i + window_size);
            jmin = (j - window_size) > 0 ? (j - window_size) : 0;
            jmax = (j + window_size) > Y_dim ? Y_dim : (j + window_size);

            // printf("Start\n");
            for (ti = 0; ti < (imax - imin); ti++){

                for(tj = 0; tj < (jmax - jmin); tj++){

                    tempsum = 0;
                    for(tk = 0; tk < Z_dim; tk++){

                        tempsum += pow((inputImg[tk][tj + jmin][ti + imin] - inputImg[tk][j][i]),fraction);

                    }
                    
                    distfrac[ti*(jmax - jmin) + tj] = pow(tempsum, 1/fraction);

                }

            }
            // printf("End\n");
            n_elements = (imax - imin) * (jmax - jmin);
            sort(distfrac, distfrac + n_elements);
            if(n_elements % 2 == 0)
                outputImg[j][i] = (distfrac[(n_elements-1)/2] + distfrac[n_elements/2])/2.0;
            else
                outputImg[j][i] = distfrac[n_elements/2];


        }

    }
    double ttime = omp_get_wtime() - start_time;
        
    printf("\n");
    printf("Time taken for computation  % g (s)\n", ttime);

    GDALDataset *poDstDS;
    
        
    GDALDriver *poDriver;
    poDriver = GetGDALDriverManager()->GetDriverByName(poDataset->GetDriver()->GetDescription());


    //Create output file and write to it
    char **papszOptions = NULL;
    poDstDS = poDriver->Create( argv[3], X_dim, Y_dim, 1, GDT_Float64, NULL);
    poBand = poDstDS->GetRasterBand(1);
    for (j = 0; j < Y_dim; j++)
    {
            poBand->RasterIO( GF_Write, 0, j, X_dim, 1, outputImg[j], X_dim, 1, GDT_Float32, 0, 0 );
            CPLFree(outputImg[j]);
    }
        

    //Close files and free memory
    GDALClose( (GDALDatasetH) poDataset );
    GDALClose( (GDALDatasetH) poDstDS );

    for (i = 0; i < Z_dim; i++)
    {
        for (j = 0; j < Y_dim ; j++)
        {
                CPLFree(inputImg[i][j]);
        }
        CPLFree(inputImg[i]);
    }
    CPLFree(inputImg);

    return 0;
}