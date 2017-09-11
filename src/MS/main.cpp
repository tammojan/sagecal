/*
 *
 Copyright (C) 2006-2008 Sarod Yatawatta <sarod@users.sf.net>  
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 $Id$
 */

#include "data.h"
#include <fstream>
#include <vector> 
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include<sagecal.h>
#ifndef LMCUT
#define LMCUT 40
#endif


using namespace std;
using namespace Data;

void
print_copyright(void) {
  cout<<"SAGECal 0.4.5 (C) 2011-2016 Sarod Yatawatta"<<endl;
}


void
print_help(void) {
   cout << "Usage:" << endl;
   cout<<"sagecal -d MS -s sky.txt -c cluster.txt"<<endl;
   cout<<"or"<<endl;
   cout<<"sagecal -f MSlist -s sky.txt -c cluster.txt"<<endl;
   cout << "-d MS name" << endl;
   cout << "-f MSlist: text file with MS names" << endl;
   cout << "-s sky.txt: sky model file"<< endl;
   cout << "-c cluster.txt: cluster file"<< endl;
   cout << "-p solutions.txt: if given, save solution in this file"<< endl;
   cout << "-F sky model format: 0: LSM, 1: LSM with 3 order spectra : default "<< Data::format<<endl;
   cout << "-I input column (DATA/CORRECTED_DATA/...) : default " <<Data::DataField<< endl;
   cout << "-O ouput column (DATA/CORRECTED_DATA/...) : default " <<Data::OutField<< endl;
   cout << "-e max EM iterations : default " <<Data::max_emiter<< endl;
   cout << "-g max iterations  (within single EM) : default " <<Data::max_iter<< endl;
   cout << "-l max LBFGS iterations : default " <<Data::max_lbfgs<< endl;
   cout << "-m LBFGS memory size : default " <<Data::lbfgs_m<< endl;
   cout << "-n no of worker threads : default "<<Data::Nt << endl;
   cout << "-t tile size : default " <<Data::TileSize<< endl;
   cout << "-a 0,1,2 : if 1, only simulate, if 2, simulate and add to residual, (multiplied by solutions if solutions file is also given): default " <<Data::DoSim<< endl;
   cout << "-z ignore_clusters: if only doing a simulation, ignore the cluster ids listed in this file" << endl;
   cout << "-b 0,1 : if 1, solve for each channel: default " <<Data::doChan<< endl;
   cout << "-B 0,1 : if 1, predict array beam: default " <<Data::doBeam<< endl;
   cout << "-x exclude baselines length (lambda) lower than this in calibration : default "<<Data::min_uvcut << endl;
   cout << "-y exclude baselines length (lambda) higher than this in calibration : default "<<Data::max_uvcut << endl;
   cout <<endl<<"Advanced options:"<<endl;
   cout << "-k cluster_id : correct residuals with solution of this cluster : default "<<Data::ccid<< endl;
   cout << "-o robust rho, robust matrix inversion during correction: default "<<Data::rho<< endl;
   cout << "-J 0,1 : if >0, use phase only correction: default "<<Data::phaseOnly<< endl;
   cout << "-j 0,1,2... 0 : OSaccel, 1 no OSaccel, 2: OSRLM, 3: RLM, 4: RTR, 5: RRTR, 6:NSD : default "<<Data::solver_mode<< endl;
   cout << "-L robust nu, lower bound: default "<<Data::nulow<< endl;
   cout << "-H robust nu, upper bound: default "<<Data::nuhigh<< endl;
   cout << "-W pre-whiten data: default "<<Data::whiten<< endl;
   cout << "-R randomize iterations: default "<<Data::randomize<< endl;
   cout << "-D 0,1,2 : if >0, enable diagnostics (Jacobian Leverage) 1 replace Jacobian Leverage as output, 2 only fractional noise/leverage is printed: default " <<Data::DoDiag<< endl;
   cout <<"Report bugs to <sarod@users.sf.net>"<<endl;

}

void 
ParseCmdLine(int ac, char **av) {
    print_copyright();
    char c;
    if(ac < 2)
    {
        print_help();
        exit(0);
    }
    while((c=getopt(ac, av, "a:b:c:d:e:f:g:j:k:l:m:n:o:p:s:t:x:y:z:B:D:F:I:J:O:L:H:R:W:h"))!= -1)
    {
        switch(c)
        {
            case 'd':
                TableName = optarg;
                break;
            case 'f':
                MSlist=optarg;
                break;
            case 's':
                SkyModel= optarg;
                break;
            case 'c':
                Clusters= optarg;
                break;
            case 'p':
                solfile= optarg;
                break;
            case 'g':
                max_iter= atoi(optarg);
                break;
            case 'a':
                DoSim= atoi(optarg);
                if (DoSim<0) { DoSim=1; }
                break;
            case 'b':
                doChan= atoi(optarg);
                if (doChan>1) { doChan=1; }
                break;
            case 'B':
                doBeam= atoi(optarg);
                if (doBeam>1) { doBeam=1; }
                break;
            case 'F':
                format= atoi(optarg);
                if (format>1) { format=1; }
                break;
            case 'e':
                max_emiter= atoi(optarg);
                break;
            case 'l':
                max_lbfgs= atoi(optarg);
                break;
            case 'm':
                lbfgs_m= atoi(optarg);
                break;
            case 'j':
                solver_mode= atoi(optarg);
                break;
            case 't':
                TileSize = atoi(optarg);
                break;
            case 'I': 
                DataField = optarg;
                break;
            case 'O': 
                OutField = optarg;
                break;
            case 'n': 
                Nt= atoi(optarg);
                break;
            case 'k': 
                ccid= atoi(optarg);
                break;
            case 'o': 
                rho= atof(optarg);
                break;
            case 'L': 
                nulow= atof(optarg);
                break;
            case 'H': 
                nuhigh= atof(optarg);
                break;
            case 'R': 
                randomize= atoi(optarg);
                break;
            case 'W': 
                whiten= atoi(optarg);
                break;
            case 'J': 
                phaseOnly= atoi(optarg);
                break;
            case 'x': 
                Data::min_uvcut= atof(optarg);
                break;
            case 'y': 
                Data::max_uvcut= atof(optarg);
                break;
            case 'z':
                ignorefile= optarg;
                break;
            case 'D':
                DoDiag= atoi(optarg);
                if (DoDiag<0) { DoDiag=0; }
                break;
            case 'h': 
                print_help();
                exit(1);
            default:
                print_help();
                exit(1);
        }
    }

    if (TableName) {
     cout<<" MS: "<<TableName<<endl;
    } else if (MSlist) {
     cout<<" MS list: "<<MSlist<<endl;
    } else {
     print_help();
     exit(1);
    }
    cout<<"Selecting baselines > "<<min_uvcut<<" and < "<<max_uvcut<<" wavelengths."<<endl;
    if (!DoSim) {
    cout<<"Using ";
    if (solver_mode==SM_LM_LBFGS || solver_mode==SM_OSLM_LBFGS || solver_mode==SM_RTR_OSLM_LBFGS ||  solver_mode==SM_NSD_RLBFGS) {
     cout<<"Gaussian noise model for solver."<<endl;
    } else {
     cout<<"Robust noise model for solver with degrees of freedom ["<<nulow<<","<<nuhigh<<"]."<<endl;
    }
    } else {
     cout<<"Only doing simulation (with possible correction for cluster id "<<ccid<<")."<<endl;
    }
}


int 
main(int argc, char **argv) {
    ParseCmdLine(argc, argv);

    if (!Data::SkyModel || !Data::Clusters || !(Data::TableName || Data::MSlist)) {
      print_help();
      exit(1);
    }
    Data::IOData iodata;
    Data::LBeam beam;
    iodata.tilesz=Data::TileSize;
    iodata.deltat=1.0;
    vector<string> msnames;
    if (Data::MSlist) {
     Data::readMSlist(Data::MSlist,&msnames);
    }
    if (Data::TableName) {
     if (!doBeam) {
      Data::readAuxData(Data::TableName,&iodata);
     } else {
      Data::readAuxData(Data::TableName,&iodata,&beam);
     }
     cout<<"Only one MS"<<endl;
    } else if (Data::MSlist) {
     Data::readAuxDataList(msnames,&iodata);
     cout<<"Total MS "<<msnames.size()<<endl;
    }
    fflush(stdout);
    if (Data::randomize) {
     srand(time(0)); /* use different seed */
    }

    openblas_set_num_threads(1);//Data::Nt;
    /**********************************************************/
     int M,Mt,ci,cj,ck;  
   /* parameters */
   double *p,*pinit,*pfreq;
   double **pm;
   complex double *coh;
   FILE *sfp=0;
    if (solfile) {
     if (!Data::DoSim) {
      if ((sfp=fopen(solfile,"w+"))==0) {
       fprintf(stderr,"%s: %d: no file\n",__FILE__,__LINE__);
       return 1;
      }
     } else {
     /* simulation mode, read only access */
      if ((sfp=fopen(solfile,"r"))==0) {
       fprintf(stderr,"%s: %d: no solution file present\n",__FILE__,__LINE__);
       return 1;
      }
      /* remember to skip first 3 lines from solution file */
      char chr;
      for (ci=0; ci<3; ci++) {
       do {
        chr = fgetc(sfp);
       } while (chr != '\n');
      } 
     }
    }


     double mean_nu;
     clus_source_t *carr;
     baseline_t *barr;
     read_sky_cluster(Data::SkyModel,Data::Clusters,&carr,&M,iodata.freq0,iodata.ra0,iodata.dec0,Data::format);
     /* exit if there are 0 clusters (incorrect sky model/ cluster file)*/
     if (M<=0) {
      fprintf(stderr,"%s: %d: no clusters to solve\n",__FILE__,__LINE__);
      exit(1);
     } else {
      printf("Got %d clusters\n",M);
     }

     /* array to store baseline->sta1,sta2 map */
     if ((barr=(baseline_t*)calloc((size_t)iodata.Nbase*iodata.tilesz,sizeof(baseline_t)))==0) {
      fprintf(stderr,"%s: %d: no free memory\n",__FILE__,__LINE__);
      exit(1);
     }
     generate_baselines(iodata.Nbase,iodata.tilesz,iodata.N,barr,Data::Nt);

     /* calculate actual no of parameters needed,
      this could be > M */
     Mt=0;
     for (ci=0; ci<M; ci++) {
       //printf("cluster %d has %d time chunks\n",carr[ci].id,carr[ci].nchunk);
       Mt+=carr[ci].nchunk;
     }
     printf("Total effective clusters: %d\n",Mt);

  /* parameters 8*N*M ==> 8*N*Mt */
  if ((p=(double*)calloc((size_t)iodata.N*8*Mt,sizeof(double)))==0) {
     fprintf(stderr,"%s: %d: no free memory\n",__FILE__,__LINE__);
     exit(1);
  }
   /* if simulation mode, solution file is given check if ignore file 
      is also given */
  int *ignorelist=0;
  if (Data::DoSim && solfile) {
    if ((ignorelist=(int*)calloc((size_t)M,sizeof(int)))==0) {
     fprintf(stderr,"%s: %d: no free memory\n",__FILE__,__LINE__);
     exit(1);
    }
    if (ignorefile) {
      update_ignorelist(ignorefile,ignorelist,M,carr);
    }
  }

#ifdef USE_MIC
  /* need for bitwise copyable parameter passing */
  int *mic_pindex,*mic_chunks;
  if ((mic_chunks=(int*)calloc((size_t)M,sizeof(int)))==0) {
     fprintf(stderr,"%s: %d: no free memory\n",__FILE__,__LINE__);
     exit(1);
  }
  if ((mic_pindex=(int*)calloc((size_t)Mt,sizeof(int)))==0) {
     fprintf(stderr,"%s: %d: no free memory\n",__FILE__,__LINE__);
     exit(1);
  }
  int cl=0;
#endif

  /* update cluster array with correct pointers to parameters */
  cj=0;
  for (ci=0; ci<M; ci++) {
    if ((carr[ci].p=(int*)calloc((size_t)carr[ci].nchunk,sizeof(int)))==0) {
     fprintf(stderr,"%s: %d: no free memory\n",__FILE__,__LINE__);
     exit(1);
    }
#ifdef USE_MIC
    mic_chunks[ci]=carr[ci].nchunk;
#endif
    for (ck=0; ck<carr[ci].nchunk; ck++) {
      carr[ci].p[ck]=cj*8*iodata.N;
#ifdef USE_MIC
      mic_pindex[cl++]=carr[ci].p[ck];
#endif
      cj++;
    }
  }

  /* pointers to parameters */
  if ((pm=(double**)calloc((size_t)Mt,sizeof(double*)))==0) {
     fprintf(stderr,"%s: %d: no free memory\n",__FILE__,__LINE__);
     exit(1);
  }
  /* setup the pointers */
  for (ci=0; ci<Mt; ci++) {
   pm[ci]=&(p[ci*8*iodata.N]);
  }
  /* initilize parameters to [1,0,0,0,0,0,1,0] */
  for (ci=0; ci<Mt; ci++) {
    for (cj=0; cj<iodata.N; cj++) {
      pm[ci][8*cj]=1.0;
      pm[ci][8*cj+6]=1.0;
    }
  }
  /* backup of default initial values */
  if ((pinit=(double*)calloc((size_t)iodata.N*8*Mt,sizeof(double)))==0) {
     fprintf(stderr,"%s: %d: no free memory\n",__FILE__,__LINE__);
     exit(1);
  }
  memcpy(pinit,p,(size_t)iodata.N*8*Mt*sizeof(double));

  /* coherencies */
  if ((coh=(complex double*)calloc((size_t)(M*iodata.Nbase*iodata.tilesz*4),sizeof(complex double)))==0) {
     fprintf(stderr,"%s: %d: no free memory\n",__FILE__,__LINE__);
     exit(1);
  }



    double res_0,res_1,res_00,res_01;   
   /* previous residual */
   double res_prev=CLM_DBL_MAX;
   double res_ratio=5; /* how much can the residual increase before resetting solutions */
   res_0=res_1=res_00=res_01=0.0;

    /**********************************************************/
    Block<int> sort(1);
    sort[0] = MS::TIME; /* note: only sort over TIME for ms iterator to work */
    /* timeinterval in seconds */
    cout<<"For "<<iodata.tilesz<<" samples, solution time interval (s): "<<iodata.deltat*(double)iodata.tilesz<<endl;
    cout<<"Freq: "<<iodata.freq0/1e6<<" MHz, Chan: "<<iodata.Nchan<<" Bandwidth: "<<iodata.deltaf/1e6<<" MHz"<<endl;
    vector<MSIter*> msitr;
    vector<MeasurementSet*> msvector;
    if (Data::TableName) {
      MeasurementSet *ms=new MeasurementSet(Data::TableName,Table::Update); 
      MSIter *mi=new MSIter(*ms,sort,iodata.deltat*(double)iodata.tilesz);
      msitr.push_back(mi);
      msvector.push_back(ms);
    } else if (Data::MSlist) {
     for(int cm=0; cm<iodata.Nms; cm++) {
      MeasurementSet *ms=new MeasurementSet(msnames[cm].c_str(),Table::Update); 
      MSIter *mi=new MSIter(*ms,sort,iodata.deltat*(double)iodata.tilesz);
      msitr.push_back(mi);
      msvector.push_back(ms);
     }
    }


    time_t start_time, end_time;
    double elapsed_time;

    int tilex=0;
    for(int cm=0; cm<iodata.Nms; cm++) {
      msitr[cm]->origin();
    }

    /* write additional info to solution file */
    if (solfile && !Data::DoSim) {
      fprintf(sfp,"# solution file created by SAGECal\n");
      fprintf(sfp,"# freq(MHz) bandwidth(MHz) time_interval(min) stations clusters effective_clusters\n");
      fprintf(sfp,"%lf %lf %lf %d %d %d\n",iodata.freq0*1e-6,iodata.deltaf*1e-6,(double)iodata.tilesz*iodata.deltat/60.0,iodata.N,M,Mt);
    }


    /* starting iterations are doubled */
    int start_iter=1;
    int sources_precessed=0;

    double inv_c=1.0/CONST_C;

    while (msitr[0]->more()) {
      start_time = time(0);
      if (iodata.Nms==1) {
       if (!doBeam) {
        Data::loadData(msitr[0]->table(),iodata,&iodata.fratio);
       } else {
        Data::loadData(msitr[0]->table(),iodata,beam,&iodata.fratio);
       }
      } else { 
       Data::loadDataList(msitr,iodata,&iodata.fratio);
      }
    /* rescale u,v,w by 1/c NOT to wavelengths, that is done later in prediction */
    my_dscal(iodata.Nbase*iodata.tilesz,inv_c,iodata.u);
    my_dscal(iodata.Nbase*iodata.tilesz,inv_c,iodata.v);
    my_dscal(iodata.Nbase*iodata.tilesz,inv_c,iodata.w);

    /**********************************************************/
    /* update baseline flags */
    /* and set x[]=0 for flagged values */
    preset_flags_and_data(iodata.Nbase*iodata.tilesz,iodata.flag,barr,iodata.x,Data::Nt);
    /* if data is being whitened, whiten x here,
     no need for a copy because we use xo for residual calculation */
    if (Data::whiten) {
     whiten_data(iodata.Nbase*iodata.tilesz,iodata.x,iodata.u,iodata.v,iodata.freq0,Data::Nt);
    }
    /* precess source locations (also beam pointing) from J2000 to JAPP if we do any beam predictions,
      using first time slot as epoch */
    if (doBeam && !sources_precessed) {
      precess_source_locations(beam.time_utc[iodata.tilesz/2],carr,M,&beam.p_ra0,&beam.p_dec0,Data::Nt);
      sources_precessed=1;
    }


#ifdef USE_MIC
  double *mic_u,*mic_v,*mic_w,*mic_x;
  mic_u=iodata.u;
  mic_v=iodata.v;
  mic_w=iodata.w;
  mic_x=iodata.x;
  int mic_Nbase=iodata.Nbase;
  int mic_tilesz=iodata.tilesz;
  int mic_N=iodata.N;
  double mic_freq0=iodata.freq0;
  double mic_deltaf=iodata.deltaf;
  double mic_data_min_uvcut=Data::min_uvcut;
  int mic_data_Nt=Data::Nt;
  int mic_data_max_emiter=Data::max_emiter;
  int mic_data_max_iter=Data::max_iter;
  int mic_data_max_lbfgs=Data::max_lbfgs;
  int mic_data_lbfgs_m=Data::lbfgs_m;
  int mic_data_gpu_threads=Data::gpu_threads;
  int mic_data_linsolv=Data::linsolv;
  int mic_data_solver_mode=Data::solver_mode;
  int mic_data_randomize=Data::randomize;
  double mic_data_nulow=Data::nulow;
  double mic_data_nuhigh=Data::nuhigh;
#endif

   /* FIXME: uvmin is not needed in calibration, because its taken care of by flags */
    if (!Data::DoSim) {
    /****************** calibration **************************/
////#ifndef HAVE_CUDA
    if (!doBeam) {
     precalculate_coherencies(iodata.u,iodata.v,iodata.w,coh,iodata.N,iodata.Nbase*iodata.tilesz,barr,carr,M,iodata.freq0,iodata.deltaf,iodata.deltat,iodata.dec0,Data::min_uvcut,Data::max_uvcut,Data::Nt);
    } else {
////     precalculate_coherencies_withbeam(iodata.u,iodata.v,iodata.w,coh,iodata.N,iodata.Nbase*iodata.tilesz,barr,carr,M,iodata.freq0,iodata.deltaf,iodata.deltat,iodata.dec0,Data::min_uvcut,Data::max_uvcut,
////  beam.p_ra0,beam.p_dec0,iodata.freq0,beam.sx,beam.sy,beam.time_utc,iodata.tilesz,beam.Nelem,beam.xx,beam.yy,beam.zz,Data::Nt);
////    }
////#endif
#ifdef HAVE_CUDA
     precalculate_coherencies_withbeam_gpu(iodata.u,iodata.v,iodata.w,coh,iodata.N,iodata.Nbase*iodata.tilesz,barr,carr,M,iodata.freq0,iodata.deltaf,iodata.deltat,iodata.dec0,Data::min_uvcut,Data::max_uvcut,
  beam.p_ra0,beam.p_dec0,iodata.freq0,beam.sx,beam.sy,beam.time_utc,iodata.tilesz,beam.Nelem,beam.xx,beam.yy,beam.zz,doBeam,Data::Nt);
#endif
    }

    
#ifndef HAVE_CUDA
    if (start_iter) {
#ifdef USE_MIC
    int mic_data_dochan=Data::doChan;
    #pragma offload target(mic) \
     nocopy(mic_u: length(1) alloc_if(1) free_if(0)) \
     nocopy(mic_v: length(1) alloc_if(1) free_if(0)) \
     nocopy(mic_w: length(1) alloc_if(1) free_if(0)) \
     in(mic_x: length(8*mic_Nbase*mic_tilesz)) \
     in(barr: length(mic_Nbase*mic_tilesz)) \
     in(mic_chunks: length(M)) \
     in(mic_pindex: length(Mt)) \
     in(coh: length(4*M*mic_Nbase*mic_tilesz)) \
     inout(p: length(8*mic_N*Mt)) 
     sagefit_visibilities_mic(mic_u,mic_v,mic_w,mic_x,mic_N,mic_Nbase,mic_tilesz,barr,mic_chunks,mic_pindex,coh,M,Mt,mic_freq0,mic_deltaf,p,mic_data_min_uvcut,mic_data_Nt,2*mic_data_max_emiter,mic_data_max_iter,(mic_data_dochan? 0 :mic_data_max_lbfgs),mic_data_lbfgs_m,mic_data_gpu_threads,mic_data_linsolv,mic_data_solver_mode,mic_data_nulow,mic_data_nuhigh,mic_data_randomize,&mean_nu,&res_0,&res_1);
#else /* NOT MIC */
     sagefit_visibilities(iodata.u,iodata.v,iodata.w,iodata.x,iodata.N,iodata.Nbase,iodata.tilesz,barr,carr,coh,M,Mt,iodata.freq0,iodata.deltaf,p,Data::min_uvcut,Data::Nt,(iodata.N<=LMCUT?2*Data::max_emiter:4*Data::max_emiter),Data::max_iter,(Data::doChan? 0 :Data::max_lbfgs),Data::lbfgs_m,Data::gpu_threads,Data::linsolv,(iodata.N<=LMCUT && Data::solver_mode==SM_RTR_OSLM_LBFGS?SM_OSLM_LBFGS:(iodata.N<=LMCUT && (Data::solver_mode==SM_RTR_OSRLM_RLBFGS||Data::solver_mode==SM_NSD_RLBFGS)?SM_OSLM_OSRLM_RLBFGS:Data::solver_mode)),Data::nulow,Data::nuhigh,Data::randomize,&mean_nu,&res_0,&res_1);
     //sagefit_visibilities(iodata.u,iodata.v,iodata.w,iodata.x,iodata.N,iodata.Nbase,iodata.tilesz,barr,carr,coh,M,Mt,iodata.freq0,iodata.deltaf,p,Data::min_uvcut,Data::Nt,2*Data::max_emiter,Data::max_iter,(Data::doChan? 0 :Data::max_lbfgs),Data::lbfgs_m,Data::gpu_threads,Data::linsolv,Data::solver_mode,Data::nulow,Data::nuhigh,Data::randomize,&mean_nu,&res_0,&res_1);
#endif /* USE_MIC */
     start_iter=0;
    } else {
#ifdef USE_MIC
    int mic_data_dochan=Data::doChan;
    #pragma offload target(mic) \
     nocopy(mic_u: length(1) alloc_if(1) free_if(0)) \
     nocopy(mic_v: length(1) alloc_if(1) free_if(0)) \
     nocopy(mic_w: length(1) alloc_if(1) free_if(0)) \
     in(mic_x: length(8*mic_Nbase*mic_tilesz)) \
     in(barr: length(mic_Nbase*mic_tilesz)) \
     in(mic_chunks: length(M)) \
     in(mic_pindex: length(Mt)) \
     in(coh: length(4*M*mic_Nbase*mic_tilesz)) \
     inout(p: length(8*mic_N*Mt)) 
     sagefit_visibilities_mic(mic_u,mic_v,mic_w,mic_x,mic_N,mic_Nbase,mic_tilesz,barr,mic_chunks,mic_pindex,coh,M,Mt,mic_freq0,mic_deltaf,p,mic_data_min_uvcut,mic_data_Nt,mic_data_max_emiter,mic_data_max_iter,(mic_data_dochan? 0: mic_data_max_lbfgs),mic_data_lbfgs_m,mic_data_gpu_threads,mic_data_linsolv,mic_data_solver_mode,mic_data_nulow,mic_data_nuhigh,mic_data_randomize,&mean_nu,&res_0,&res_1);
#else /* NOT MIC */
     sagefit_visibilities(iodata.u,iodata.v,iodata.w,iodata.x,iodata.N,iodata.Nbase,iodata.tilesz,barr,carr,coh,M,Mt,iodata.freq0,iodata.deltaf,p,Data::min_uvcut,Data::Nt,Data::max_emiter,Data::max_iter,(Data::doChan? 0: Data::max_lbfgs),Data::lbfgs_m,Data::gpu_threads,Data::linsolv,Data::solver_mode,Data::nulow,Data::nuhigh,Data::randomize,&mean_nu,&res_0,&res_1);
#endif /* USE_MIC */
    }
#endif /* !HAVE_CUDA */
#ifdef HAVE_CUDA
#ifdef ONE_GPU
    if (start_iter) {
     sagefit_visibilities_dual_pt_one_gpu(iodata.u,iodata.v,iodata.w,iodata.x,iodata.N,iodata.Nbase,iodata.tilesz,barr,carr,coh,M,Mt,iodata.freq0,iodata.deltaf,p,Data::min_uvcut,Data::Nt, (iodata.N<=LMCUT?2*Data::max_emiter:4*Data::max_emiter),Data::max_iter,(Data::doChan? 0: Data::max_lbfgs),Data::lbfgs_m,Data::gpu_threads,Data::linsolv,(iodata.N<=LMCUT && Data::solver_mode==SM_RTR_OSLM_LBFGS?SM_OSLM_LBFGS:(iodata.N<=LMCUT && (Data::solver_mode==SM_RTR_OSRLM_RLBFGS||Data::solver_mode==SM_NSD_RLBFGS)?SM_OSLM_OSRLM_RLBFGS:Data::solver_mode)),Data::nulow,Data::nuhigh,Data::randomize,&mean_nu,&res_0,&res_1);
     start_iter=0;
    } else {
     sagefit_visibilities_dual_pt_one_gpu(iodata.u,iodata.v,iodata.w,iodata.x,iodata.N,iodata.Nbase,iodata.tilesz,barr,carr,coh,M,Mt,iodata.freq0,iodata.deltaf,p,Data::min_uvcut,Data::Nt,Data::max_emiter,Data::max_iter,(Data::doChan? 0:Data::max_lbfgs),Data::lbfgs_m,Data::gpu_threads,Data::linsolv,Data::solver_mode,Data::nulow,Data::nuhigh,Data::randomize,&mean_nu,&res_0,&res_1);
    }
#endif /* ONE_GPU */
#ifndef ONE_GPU
    if (start_iter) {
     sagefit_visibilities_dual_pt_flt(iodata.u,iodata.v,iodata.w,iodata.x,iodata.N,iodata.Nbase,iodata.tilesz,barr,carr,coh,M,Mt,iodata.freq0,iodata.deltaf,p,Data::min_uvcut,Data::Nt,(iodata.N<=LMCUT?2*Data::max_emiter:4*Data::max_emiter),Data::max_iter,(Data::doChan? 0:Data::max_lbfgs),Data::lbfgs_m,Data::gpu_threads,Data::linsolv,(iodata.N<=LMCUT && Data::solver_mode==SM_RTR_OSLM_LBFGS?SM_OSLM_LBFGS:(iodata.N<=LMCUT && (Data::solver_mode==SM_RTR_OSRLM_RLBFGS||Data::solver_mode==SM_NSD_RLBFGS)?SM_OSLM_OSRLM_RLBFGS:Data::solver_mode)),Data::nulow,Data::nuhigh,Data::randomize,&mean_nu,&res_0,&res_1);
     ///DBG sagefit_visibilities_dual_pt_flt(iodata.u,iodata.v,iodata.w,iodata.x,iodata.N,iodata.Nbase,iodata.tilesz,barr,carr,coh,M,Mt,iodata.freq0,iodata.deltaf,p,Data::min_uvcut,Data::Nt,2*Data::max_emiter,Data::max_iter,(Data::doChan? 0:Data::max_lbfgs),Data::lbfgs_m,Data::gpu_threads,Data::linsolv,Data::solver_mode,Data::nulow,Data::nuhigh,Data::randomize,&mean_nu,&res_0,&res_1);
     start_iter=0;
    } else {
     sagefit_visibilities_dual_pt_flt(iodata.u,iodata.v,iodata.w,iodata.x,iodata.N,iodata.Nbase,iodata.tilesz,barr,carr,coh,M,Mt,iodata.freq0,iodata.deltaf,p,Data::min_uvcut,Data::Nt,Data::max_emiter,Data::max_iter,(Data::doChan? 0:Data::max_lbfgs),Data::lbfgs_m,Data::gpu_threads,Data::linsolv,Data::solver_mode,Data::nulow,Data::nuhigh,Data::randomize,&mean_nu,&res_0,&res_1);
    }
#endif /* !ONE_GPU */
#endif /* HAVE_CUDA */
   /* if multi channel mode, run BFGS for each channel here 
       and then calculate residuals, else just calculate residuals */
      /* parameters 8*N*M ==> 8*N*Mt */
    if (Data::doChan) {
      if ((pfreq=(double*)calloc((size_t)iodata.N*8*Mt,sizeof(double)))==0) {
        fprintf(stderr,"%s: %d: no free memory\n",__FILE__,__LINE__);
        exit(1);
      }
      double *xfreq;
      if ((xfreq=(double*)calloc((size_t)iodata.Nbase*iodata.tilesz*8,sizeof(double)))==0) {
        fprintf(stderr,"%s: %d: no free memory\n",__FILE__,__LINE__);
        exit(1);
      }

      double deltafch=iodata.deltaf/(double)iodata.Nchan;
      for (ci=0; ci<iodata.Nchan; ci++) {
        memcpy(pfreq,p,(size_t)iodata.N*8*Mt*sizeof(double));
        memcpy(xfreq,&iodata.xo[ci*iodata.Nbase*iodata.tilesz*8],(size_t)iodata.Nbase*iodata.tilesz*8*sizeof(double));
        precalculate_coherencies(iodata.u,iodata.v,iodata.w,coh,iodata.N,iodata.Nbase*iodata.tilesz,barr,carr,M,iodata.freqs[ci],deltafch,iodata.deltat,iodata.dec0,Data::min_uvcut,Data::max_uvcut,Data::Nt);
      /* FIT, and calculate */
#ifndef HAVE_CUDA
#ifdef USE_MIC
        mic_freq0=iodata.freqs[ci];
        mic_deltaf=deltafch;
     #pragma offload target(mic) \
      nocopy(mic_u: length(1) alloc_if(1) free_if(0)) \
      nocopy(mic_v: length(1) alloc_if(1) free_if(0)) \
      nocopy(mic_w: length(1) alloc_if(1) free_if(0)) \
      in(xfreq: length(8*mic_Nbase*mic_tilesz)) \
      in(barr: length(mic_Nbase*mic_tilesz)) \
      in(mic_chunks: length(M)) \
      in(mic_pindex: length(Mt)) \
      in(coh: length(4*M*mic_Nbase*mic_tilesz)) \
      inout(pfreq: length(8*mic_N*Mt)) 
        bfgsfit_visibilities_mic(mic_u,mic_v,mic_w,xfreq,mic_N,mic_Nbase,mic_tilesz,barr,mic_chunks,mic_pindex,coh,M,Mt,mic_freq0,mic_deltaf,pfreq,mic_data_min_uvcut,mic_data_Nt,mic_data_max_lbfgs,mic_data_lbfgs_m,mic_data_gpu_threads,mic_data_solver_mode,mean_nu,&res_00,&res_01);
        mic_freq0=iodata.freq0;
        mic_deltaf=iodata.deltaf;
#else /* NOT MIC */
        bfgsfit_visibilities(iodata.u,iodata.v,iodata.w,xfreq,iodata.N,iodata.Nbase,iodata.tilesz,barr,carr,coh,M,Mt,iodata.freqs[ci],deltafch,pfreq,Data::min_uvcut,Data::Nt,Data::max_lbfgs,Data::lbfgs_m,Data::gpu_threads,Data::solver_mode,mean_nu,&res_00,&res_01);
#endif /* USE_MIC */
#endif /* !HAVE_CUDA */
#ifdef HAVE_CUDA
        bfgsfit_visibilities_gpu(iodata.u,iodata.v,iodata.w,xfreq,iodata.N,iodata.Nbase,iodata.tilesz,barr,carr,coh,M,Mt,iodata.freqs[ci],deltafch,pfreq,Data::min_uvcut,Data::Nt,Data::max_lbfgs,Data::lbfgs_m,Data::gpu_threads,Data::solver_mode,mean_nu,&res_00,&res_01);
#endif /* HAVE_CUDA */
        calculate_residuals(iodata.u,iodata.v,iodata.w,pfreq,&iodata.xo[ci*iodata.Nbase*iodata.tilesz*8],iodata.N,iodata.Nbase,iodata.tilesz,barr,carr,M,iodata.freqs[ci],deltafch,iodata.deltat,iodata.dec0,Data::Nt,Data::ccid,Data::rho);
      }
      /* use last solution to save as output */
      memcpy(p,pfreq,(size_t)iodata.N*8*Mt*sizeof(double));
      free(pfreq);
      free(xfreq);
    } else {
     /* since residual is calculated not using x (instead using xo), no need to unwhiten data
        in case x was whitened */
     if (!doBeam) {
      calculate_residuals_multifreq(iodata.u,iodata.v,iodata.w,p,iodata.xo,iodata.N,iodata.Nbase,iodata.tilesz,barr,carr,M,iodata.freqs,iodata.Nchan,iodata.deltaf,iodata.deltat,iodata.dec0,Data::Nt,Data::ccid,Data::rho,Data::phaseOnly);
     } else {
      calculate_residuals_multifreq_withbeam(iodata.u,iodata.v,iodata.w,p,iodata.xo,iodata.N,iodata.Nbase,iodata.tilesz,barr,carr,M,iodata.freqs,iodata.Nchan,iodata.deltaf,iodata.deltat,iodata.dec0,
beam.p_ra0,beam.p_dec0,iodata.freq0,beam.sx,beam.sy,beam.time_utc,beam.Nelem,beam.xx,beam.yy,beam.zz,Data::Nt,Data::ccid,Data::rho,Data::phaseOnly);
     }
    }
    /****************** end calibration **************************/
    /****************** begin diagnostics ************************/
#ifdef HAVE_CUDA
    if (Data::DoDiag) {
     calculate_diagnostics(iodata.u,iodata.v,iodata.w,p,iodata.xo,iodata.N,iodata.Nbase,iodata.tilesz,barr,carr,coh,M,Mt,Data::DoDiag,Data::Nt);
    }
#endif /* HAVE_CUDA */
    /****************** end diagnostics **************************/
   } else {
    /************ simulation only mode ***************************/
    if (!solfile) {
////#ifndef HAVE_CUDA
     if (!doBeam) {
      predict_visibilities_multifreq(iodata.u,iodata.v,iodata.w,iodata.xo,iodata.N,iodata.Nbase,iodata.tilesz,barr,carr,M,iodata.freqs,iodata.Nchan,iodata.deltaf,iodata.deltat,iodata.dec0,Data::Nt,(Data::DoSim>1?1:0));
     } else {
      predict_visibilities_multifreq_withbeam(iodata.u,iodata.v,iodata.w,iodata.xo,iodata.N,iodata.Nbase,iodata.tilesz,barr,carr,M,iodata.freqs,iodata.Nchan,iodata.deltaf,iodata.deltat,iodata.dec0,
  beam.p_ra0,beam.p_dec0,iodata.freq0,beam.sx,beam.sy,beam.time_utc,beam.Nelem,beam.xx,beam.yy,beam.zz, Data::Nt,(Data::DoSim>1?1:0));
     }
////#endif
////#ifdef HAVE_CUDA
////      predict_visibilities_multifreq_withbeam_gpu(iodata.u,iodata.v,iodata.w,iodata.xo,iodata.N,iodata.Nbase,iodata.tilesz,barr,carr,M,iodata.freqs,iodata.Nchan,iodata.deltaf,iodata.deltat,iodata.dec0,
////  beam.p_ra0,beam.p_dec0,iodata.freq0,beam.sx,beam.sy,beam.time_utc,beam.Nelem,beam.xx,beam.yy,beam.zz,doBeam,Data::Nt,(Data::DoSim>1?1:0));
////#endif
    } else {
     read_solutions(sfp,p,carr,iodata.N,M);
    /* if solution file is given, read in the solutions and predict */
    predict_visibilities_multifreq_withsol(iodata.u,iodata.v,iodata.w,p,iodata.xo,ignorelist,iodata.N,iodata.Nbase,iodata.tilesz,barr,carr,M,iodata.freqs,iodata.Nchan,iodata.deltaf,iodata.deltat,iodata.dec0,Data::Nt,(Data::DoSim>1?1:0),Data::ccid,Data::rho,Data::phaseOnly);
    }
    /************ end simulation only mode ***************************/
   }

   tilex+=iodata.tilesz;
   /* print solutions to file */
   if (!Data::DoSim && solfile) {
    for (cj=0; cj<iodata.N*8; cj++) {
     fprintf(sfp,"%d ",cj);
     for (ci=M-1; ci>=0; ci--) {
       for (ck=0; ck<carr[ci].nchunk; ck++) {
        fprintf(sfp," %e",p[carr[ci].p[ck]+cj]);
       }
     }
     fprintf(sfp,"\n");
    }
   }

    /**********************************************************/
      /* also write back */
    if (iodata.Nms==1) {
     Data::writeData(msitr[0]->table(),iodata);
    } else {
     Data::writeDataList(msitr,iodata);
    }
    for(int cm=0; cm<iodata.Nms; cm++) {
      (*msitr[cm])++;
    }
   if (!Data::DoSim) {
   /* if residual has increased too much, or all are flagged (0 residual)
      or NaN
      reset solutions to original
      initial values */
   if (res_1==0.0 || !isfinite(res_1) || res_1>res_ratio*res_prev) {
     cout<<"Resetting Solution"<<endl;
     /* reset solutions so next iteration has default initial values */
     memcpy(p,pinit,(size_t)iodata.N*8*Mt*sizeof(double));
     /* also assume iterations have restarted from scratch */
     start_iter=1;
     /* also forget min residual (otherwise will try to reset it always) */
     res_prev=res_1;
   } else if (res_1<res_prev) { /* only store the min value */
    res_prev=res_1;
   }
   }
    end_time = time(0);
    elapsed_time = ((double) (end_time-start_time)) / 60.0;
    if (!Data::DoSim) {
    if (solver_mode==SM_OSLM_OSRLM_RLBFGS||solver_mode==SM_RLM_RLBFGS||solver_mode==SM_RTR_OSRLM_RLBFGS || solver_mode==SM_NSD_RLBFGS) { 
    cout<<"nu="<<mean_nu<<endl;
    }
      cout<<"Timeslot: "<<tilex<<" Residual: initial="<<res_0<<",final="<<res_1<<", Time spent="<<elapsed_time<<" minutes"<<endl;
    } else {
      cout<<"Timeslot: "<<tilex<<", Time spent="<<elapsed_time<<" minutes"<<endl;
    }
    }


    for(int cm=0; cm<iodata.Nms; cm++) {
     delete msitr[cm];
     delete msvector[cm];
    }

   if (!doBeam) {
    Data::freeData(iodata);
   } else {
    Data::freeData(iodata,beam);
   }

#ifdef USE_MIC
   free(mic_pindex);
   free(mic_chunks);
#endif
    /**********************************************************/

  exinfo_gaussian *exg;
  exinfo_disk *exd;
  exinfo_ring *exr;
  exinfo_shapelet *exs;

  for (ci=0; ci<M; ci++) {
    free(carr[ci].ll);
    free(carr[ci].mm);
    free(carr[ci].nn);
    free(carr[ci].sI);
    free(carr[ci].sQ);
    free(carr[ci].sU);
    free(carr[ci].sV);
    free(carr[ci].p);
    free(carr[ci].ra);
    free(carr[ci].dec);
    for (cj=0; cj<carr[ci].N; cj++) {
     /* do a proper typecast before freeing */
     switch (carr[ci].stype[cj]) {
      case STYPE_GAUSSIAN:
        exg=(exinfo_gaussian*)carr[ci].ex[cj];
        if (exg) { free(exg); carr[ci].ex[cj]=0; }
        break;
      case STYPE_DISK:
        exd=(exinfo_disk*)carr[ci].ex[cj];
        if (exd) { free(exd); carr[ci].ex[cj]=0; }
        break;
      case STYPE_RING:
        exr=(exinfo_ring*)carr[ci].ex[cj];
        if (exr) { free(exr); carr[ci].ex[cj]=0; }
        break;
      case STYPE_SHAPELET:
        exs=(exinfo_shapelet*)carr[ci].ex[cj];
        if (exs)  {
          if (exs->modes) {
            free(exs->modes);
          }
          free(exs);
          carr[ci].ex[cj]=0;
        }
        break;
      default:
        break;
     }
    }
    free(carr[ci].ex);
    free(carr[ci].stype);
    free(carr[ci].sI0);
    free(carr[ci].sQ0);
    free(carr[ci].sU0);
    free(carr[ci].sV0);
    free(carr[ci].f0);
    free(carr[ci].spec_idx);
    free(carr[ci].spec_idx1);
    free(carr[ci].spec_idx2);
  }
  free(carr);
  free(barr);
  free(p);
  free(pinit);
  free(pm);
  free(coh);
  if (solfile) {
    fclose(sfp);
  }
  if (Data::DoSim && solfile) {
    free(ignorelist);
  }
  /**********************************************************/

#ifdef HAVE_CUDA
  cudaDeviceReset();
#endif
   cout<<"Done."<<endl;    
   return 0;
}
