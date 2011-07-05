#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <sys/times.h>
#include <time.h>


using namespace std;

#define RSD(x) ((x&2)?(x|1):(x&(~1)))


	//inputs
	int hd_sd;
	int slice_type;
	int mb_type, mb_subtype;
	int mvd_x_l0, mvd_y_l0, mvd_x_l1, mvd_y_l1;
	int ref_frame_l0, ref_frame_l1;
	int mb_skip;
	int direct_spatial_mv_pred_flag;
	int short_term;
	int direct_8x8_inference_flag = 1;
	
	//internals
	int max_x = 44, max_y = 36;
	int vec_x_l0, vec_x_l1;
	int vec_y_l0, vec_y_l1;
	int block_x = 0, rBX;
	int block_y = 0, rBY;
	int mv_memory_x_l0[512];
	int mv_memory_y_l0[512];
	int ref_memory_l0[256];
	
	int mv_memory_x_l1[512];
	int mv_memory_y_l1[512];
	int ref_memory_l1[256];
	
	int pred_l0, pred_l1;
	
	int mb_mv_l0[16][2];
	int mb_ref_l0[4];
	int mb_mv_a_l0[4][2];
	int mb_mv_b_l0[4][2];
	int mb_mv_c_l0[2];
	int mb_mv_d_l0[2];
	int ref_a_l0[2] = {-1,-1};
	int ref_b_l0[2] = {-1,-1};
	int ref_c_l0 = -1;
	int ref_d_l0 = -1;

	int mb_mv_l1[16][2];
	int mb_ref_l1[4];
	int mb_mv_a_l1[4][2];
	int mb_mv_b_l1[4][2];
	int mb_mv_c_l1[2];
	int mb_mv_d_l1[2];
	int ref_a_l1[2] = {-1,-1};
	int ref_b_l1[2] = {-1,-1};
	int ref_c_l1 = -1;
	int ref_d_l1 = -1;

		
	int mv_colocated_l0[36][44][2];
	int ref_colocated_l0[36][44];
	
	int mv_colocated_l1[36][44][2];
	int ref_colocated_l1[36][44];
	
	int block_a_avail, block_b_avail, block_c_avail, block_d_avail;
	int block_a_ref_l0, block_b_ref_l0, block_c_ref_l0, block_d_ref_l0;
	int block_a_ref_l1, block_b_ref_l1, block_c_ref_l1, block_d_ref_l1;
	int idx_mv = 0;
	int idx_ref;
	int mb_x, mb_y;
	int block_a_mv_l0[2], block_b_mv_l0[2], block_c_mv_l0[2], block_d_mv_l0[2];
	int block_a_mv_l1[2], block_b_mv_l1[2], block_c_mv_l1[2], block_d_mv_l1[2];
	int mv_a_l0[2], mv_b_l0[2], mv_c_l0[2], pred_vec_l0[2]={0,0};
	int mv_a_l1[2], mv_b_l1[2], mv_c_l1[2], pred_vec_l1[2]={0,0};
	int mvPredType_l0, rFrameL_l0, rFrameU_l0, rFrameUR_l0;
	int mvPredType_l1, rFrameL_l1, rFrameU_l1, rFrameUR_l1;
	
	const int MVPRED_MEDIAN = 0;
	const int MVPRED_L      = 1;
	const int MVPRED_U      = 2;
	const int MVPRED_UR     = 3;
	int blockshape_x = 16;
	int blockshape_y = 16;
	int idx;
	
	int mb_refcol_l0, mb_refcol_l1;
	int mb_mvcol_l0[2], mb_mvcol_l1[2];
	int colZeroFlag, directZeroPredictionFlag;
	int Dir_ref_frame_l0,  Dir_ref_frame_l1;
	int Dir_pred_l0, Dir_pred_l1;
	
	int Dir_pred_vec_l0[2], Dir_pred_vec_l1[2];
	bool dir_pred;
	
	int VecZeroFlag_L0 = 0;
    int VecZeroFlag_L1 = 0;
    
    int DistScaleFactor = 0;
    
	
	//Simulador
	int mb_subtypev[4];
	int rd_mv_l0[16][2];
	int rd_mv_l1[16][2];
	int rd_ref_l0[4];
	int rd_ref_l1[4];
	int ref_temp_l0[4], ref_temp_l1[4];
	int part_idx, sub_part_idx;
	int temp, icounter=0, counter = 0;
	int temp_vec_l0[16][2];
	int temp_vec_l1[16][2];
	int idx_tvl0, idx_tvl1;
	int def_ref;
	char trash;
	bool intra;	
	
	const int MBBLOCK_STEP[8][2]	= {{4,4},{4,4},{4,2},{2,4},{2,2},{2,1},{1,2},{1,1}};//% change
	const int BLOCK_STEP[8][2]		= {{2,2},{2,1},{1,2},{1,1},{2,2},{2,1},{1,2},{1,1}};
	const int BSMBBLOCK_STEP[23][2]	= {{2,2},{4,4},{4,4},{4,4},{4,2},{2,4},{4,2},{2,4},{4,2},{2,4},{4,2},{2,4},{4,2},{2,4},{4,2},{2,4},{4,2},{2,4},{4,2},{2,4},{4,2},{2,4},{2,2}};//% change
	const int BSBLOCK_STEP[13][2]	= {{1,1},{2,2},{2,2},{2,2},{2,1},{1,2},{2,1},{1,2},{2,1},{1,2},{1,1},{1,1},{1,1}};
	const int MB_PRED[23][2]		= {{-1,-1},{0,-1},{1,-1},{2,-1},{0,0},{0,0},{1,1},{1,1},{0,1},{0,1},{1,0},{1,0},{0,2},{0,2},{1,2},{1,2},{2,0},{2,0},{2,1},{2,1},{2,2},{2,2},{-1,-1}};	
	const int BLOCK_PRED[14]		= {-1, -1, 0, 1, 2, 0, 0, 1, 1, 2, 2, 0, 1, 2};
	const int partQuant[5] 			= {1,1,2,2,4};
	const int BSpartQuant[23]		= {4,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,4};	
	const int subpartQuant[5] 		= {1,1,2,2,4};
	const int BSsubpartQuant[14] 	= {1,4,1,1,1,2,2,2,2,2,2,4,4,4};
	const int Block_Order[16] 		= {0,1,4,5,2,3,6,7,8,9,12,13,10,11,14,15};

	
void in_type(){ cin >> trash >> mb_type >> temp >> temp;} 

void in_subtype()
{
	//cout << "\nentre mb_subtype"; 
			cin >> trash >> mb_subtypev[0];
			//cout << "\nentre mb_subtype"; 
			cin >> trash >> mb_subtypev[1];
			//cout << "\nentre mb_subtype"; 
			cin >> trash >> mb_subtypev[2];
			//cout << "\nentre mb_subtype"; 
			cin >> trash >> mb_subtypev[3];}


int clip(int inf, int sup, int n)
{
if (n > sup) n = sup;
else if (n < inf) n = inf;

return n;        
}

void read_I()
{

	for (int ii=0;ii < 99 ; ii++)
	    cin >> trash >> temp >> temp >> temp;
	}
	
void inicializa()
{
	for (int ii=0;ii < 256 ; ii++)
		ref_memory_l0[ii] = -1;
	for (int ii=0;ii < 512 ; ii++)
		mv_memory_x_l0[ii] = -1;
	for (int ii=0;ii < 512 ; ii++)
		mv_memory_y_l0[ii] = -1;
	}
	
void in_ref_l0(int A)
	  { 		cin >> trash >> ref_temp_l0[A];}
	  
void in_ref_l1(int A)
	  { 		cin >> trash >> ref_temp_l1[A];}

	  
void in_temp_vec_l0()
	{cin >> trash >> temp_vec_l0[idx_tvl0][0]; cin >> trash >> temp_vec_l0[idx_tvl0++][1];}

void in_temp_vec_l1()	
	{cin >> trash >> temp_vec_l1[idx_tvl1][0]; cin >> trash >> temp_vec_l1[idx_tvl1++][1];}
	  
void in_trash() {cin >> trash;}
	  
void read_ref_l0(int A)
	 { cin >> rd_ref_l0[A];}
	 
void read_ref_l1(int A)
	 { cin >> rd_ref_l1[A];}
	 
void read_mv_l0(int A)
          {  	         
	          cin >> rd_mv_l0[A][0] >> rd_mv_l0[A][1];}
          
void read_mv_l1(int A)
          { cin >> rd_mv_l1[A][0] >> rd_mv_l1[A][1];}
	  
	  
	  
void mbp_pos(int slice_type, int mb_type, int part_idx, int *x, int *y)
{
	part_idx+=1;

	if (slice_type == 1)
	{
		if (mb_type == 0)
			mb_type = 4;
		else if (mb_type < 4)
			mb_type = 1;
		else if (mb_type >= 4 && mb_type <= 21 && mb_type%2==0)
			mb_type = 2;	
		else if (mb_type >= 4 && mb_type <= 21 && mb_type%2==1)
			mb_type = 3;
		else
			mb_type = 4;
	}
	
	if(mb_type == 0 || mb_type == 1)
	{ 
		*x = 0; *y =0;
	}else if (mb_type == 2)
	{
		if (part_idx == 0)
		{
			*x=0; *y=0;
		}
		else
		{	
			*x=0; *y=2;
		}
	}else if (mb_type == 3)
	{
		if (part_idx == 0)
		{
			*x=0; *y=0;
		}
		else
		{	
			*x=2; *y=0;
		}
	}
	else
	{
		if (part_idx == 0)
		{
			*x=0; *y=0;
		}
		else if (part_idx == 1)
		{	
			*x=2; *y=0;
		}
		else if (part_idx == 2)
		{	
			*x=0; *y=2;
		}
		else if (part_idx == 3)
		{	
			*x=2; *y=2;
		}
	}	
}	 


void bp_pos(int slice_type, int mb_type, int part_idx, int *x, int *y)
{
	part_idx+=1;
	
	if (slice_type == 1)
	{
		if (mb_type <= 0)
			mb_type = 3;
		else if (mb_type < 4)
			mb_type = 0;
		else if (mb_type >= 4 && mb_type <= 9 && mb_type%2==0)
			mb_type = 1;	
		else if (mb_type >= 4 && mb_type <= 9 && mb_type%2==1)
			mb_type = 2;
		else
			mb_type = 3;
	}
	
	if(mb_type <= 0)
	{ 
		*x = 0; *y =0;
	}else if (mb_type == 1)
	{
		if (part_idx == 0)
		{
			*x=0; *y=0;
		}
		else
		{	
			*x=0; *y=1;
		}
	}else if (mb_type == 2)
	{
		if (part_idx == 0)
		{
			*x=0; *y=0;
		}
		else
		{	
			*x=1; *y=0;
		}
	}
	else
	{
		if (part_idx == 0)
		{
			*x=0; *y=0;
		}
		else if (part_idx == 1)
		{	
			*x=1; *y=0;
		}
		else if (part_idx == 2)
		{	
			*x=0; *y=1;
		}
		else if (part_idx == 3)
		{	
			*x=1; *y=1;
		}
	}	
}

int main(int argc, char *argv[])
{

	
/*	//timer
	struct tms before, after;

    times(&before);
    
    struct tm *local, *local1;
	time_t t;
	time_t t1;
	t= time(NULL);	
	local=localtime(&t);
	*/
	 


	
	// *watch_x = *mv_memory_x_l0;
	// *watch_y = *mv_memory_y_l0;
	
	read_I();
	
	inicializa();
	
		
	//cout << "entre hd_sd"; cin >> hd_sd;
	//cout << "\nentre slice_type"; cin >> slice_type;
	
	slice_type = 0;
	direct_spatial_mv_pred_flag = 1;//%ERROR
	short_term = 1;//%ERROR
	
	while(slice_type != -1)
	{
		rBX = block_x;
		rBY = block_y;
		//cout << "\n[" << block_y <<","<<  block_x <<"]";     
		//cout << "\nentre mb_type Img"<< icounter << " MB "<< counter++; 
		counter++;
		in_type();
		
		//cout<< "MB_type" << mb_type;
		intra = false;
		if ((mb_type > 4 && slice_type == 0) || (mb_type > 22 && slice_type == 1)) //Teste Intra
		{
			mb_type = 0;
			def_ref = -1;
			intra = true;
		}
		else def_ref = 0; 
		
		if ((mb_type != 4 && slice_type == 0) || (mb_type!= 0 && mb_type != 22 && slice_type == 1)) 
		{				
			mb_subtypev[0] = -1;
			mb_subtypev[1] = -1;
			mb_subtypev[2] = -1;
			mb_subtypev[3] = -1;
		} 
		else if (mb_type == 0 && slice_type == 1)
		{
			mb_subtypev[0] = 0;
			mb_subtypev[1] = 0;
			mb_subtypev[2] = 0;
			mb_subtypev[3] = 0;
		}
		else {
			in_subtype();
			//cout << "\nentre mb_subtype"; 
			//cin >> trash >> mb_subtypev[0];
			//cout << "\nentre mb_subtype"; 
			//cin >> trash >> mb_subtypev[1];
			//cout << "\nentre mb_subtype"; 
			//cin >> trash >> mb_subtypev[2];
			//cout << "\nentre mb_subtype"; 
			//cin >> trash >> mb_subtypev[3];
		}
        
        if(mb_type)
        {
        	for (int ii=0; ii<4; ii++){
	    		//cout << "\nref frame l0"; 
	    		in_ref_l0(ii);}
	    		//cin >> trash >> ref_temp_l0[ii]; }
	    	for (int ii=0; ii<4; ii++){
	    		//cout << "\nref frame l1"; 
	    		in_ref_l1(ii);}
	    		//cin >> trash >> ref_temp_l1[ii]; }              	           
    	}
    	else
    	{ 	
	    	for (int ii=0; ii<4; ii++){
	    		ref_temp_l0[ii] = def_ref;
	    		ref_temp_l1[ii] = def_ref;  
    		}
		}
		//****DIRECT PREDICTION
		
		VecZeroFlag_L0 = 0;
        VecZeroFlag_L1 = 0;
		
        dir_pred = false;
		if (slice_type == 1 && 
			(mb_type == 0 || (mb_type == 22 && !(mb_subtypev[0] && mb_subtypev[1] && mb_subtypev[2] && mb_subtypev[3]))))
  		{
	  		dir_pred = true;
    		if (direct_spatial_mv_pred_flag)// || !mb_type)
    		{
	    		block_a_avail = (block_x != 0)?1:0;
				block_b_avail = (block_y != 0)?1:0; 
				block_c_avail = (block_b_avail && (block_x + 4 < max_x))?1:0;
				block_d_avail = (block_a_avail && block_b_avail)?1:0;
				
				block_a_ref_l0  = ref_a_l0[0];
				block_a_ref_l1  = ref_a_l1[0];
				block_b_ref_l0  = ref_b_l0[0];
	    		block_b_ref_l1  = ref_b_l1[0];
	    		block_c_ref_l0 	= ref_c_l0;
	    		block_c_ref_l1 	= ref_c_l1;
	    		block_d_ref_l0  = ref_d_l0;
	    		block_d_ref_l1  = ref_d_l1;
	    		
	    		rFrameL_l0    = block_a_avail    ? block_a_ref_l0 : -1;
	            rFrameU_l0    = block_b_avail    ? block_b_ref_l0 : -1;
	            rFrameUR_l0   = block_c_avail    ? block_c_ref_l0 : block_d_avail ? block_d_ref_l0 : -1;
	            
	            rFrameL_l1    = block_a_avail    ? block_a_ref_l1 : -1;
	            rFrameU_l1    = block_b_avail    ? block_b_ref_l1 : -1;
	            rFrameUR_l1   = block_c_avail    ? block_c_ref_l1 : block_d_avail ? block_d_ref_l1 : -1;
	    		
	            Dir_ref_frame_l0 = (rFrameL_l0 >= 0 && rFrameU_l0 >= 0)    ? min(rFrameL_l0,rFrameU_l0): max(rFrameL_l0,rFrameU_l0);
	            Dir_ref_frame_l0 = (Dir_ref_frame_l0 >= 0 && rFrameUR_l0 >= 0) ? min(Dir_ref_frame_l0,rFrameUR_l0): max(Dir_ref_frame_l0,rFrameUR_l0);
	            
	            Dir_ref_frame_l1 = (rFrameL_l1 >= 0 && rFrameU_l1 >= 0)    ? min(rFrameL_l1,rFrameU_l1): max(rFrameL_l1,rFrameU_l1);
	            Dir_ref_frame_l1 = (Dir_ref_frame_l1 >= 0 && rFrameUR_l1 >= 0) ? min(Dir_ref_frame_l1,rFrameUR_l1): max(Dir_ref_frame_l1,rFrameUR_l1);
	            
	    		Dir_pred_l0 = (Dir_ref_frame_l0 >=0)? 1 : 0;
	    		Dir_pred_l1 = (Dir_ref_frame_l1 >=0)? 1 : 0;		  					
     
			    directZeroPredictionFlag = (!Dir_pred_l0 && !Dir_pred_l1)? 1:0;   	
			    
			    Dir_ref_frame_l0 = (!Dir_pred_l0 && !Dir_pred_l1)? 0:Dir_ref_frame_l0;
			    Dir_ref_frame_l1 = (!Dir_pred_l0 && !Dir_pred_l1)? 0:Dir_ref_frame_l1;
			    
			    /*
	    		idx = block_y * 36 + block_x;
				idx_ref = (block_y - block_y%2) * 18 + (block_x - block_x%2)/2;
	    		
	    		mb_mvcol_l0[0] = mv_colocated_l0[block_y][block_x][0];
				mb_mvcol_l0[1] = mv_colocated_l0[block_y][block_x][1];
				mb_refcol_l0   = ref_colocated_l0[block_y][block_x];
					
				mb_mvcol_l1[0] = mv_colocated_l1[block_y][block_x][0];
				mb_mvcol_l1[1] = mv_colocated_l1[block_y][block_x][1];
				mb_refcol_l1   = ref_colocated_l1[block_y][block_x];
			    
			    colZeroFlag = (short_term && (( !mb_refcol_l0 && (mb_mvcol_l0[0] >= -1 && mb_mvcol_l0[0] <= 1) && (mb_mvcol_l0[1] >= -1 && mb_mvcol_l0[1] <= 1))
			    				||(mb_refcol_l0 == -1  && !mb_refcol_l1 && (mb_mvcol_l1[0] >= -1 && mb_mvcol_l1[0] <= 1) && (mb_mvcol_l1[1] >= -1 && mb_mvcol_l1[1] <= 1))))?1:0;
			    			    
				if (directZeroPredictionFlag || Dir_ref_frame_l0 < 0 || (Dir_ref_frame_l0 ==0 && colZeroFlag))
				{
					VecZeroFlag_L0 = 1;	
				}
				if (directZeroPredictionFlag || Dir_ref_frame_l1 < 0 || (Dir_ref_frame_l1 ==0 && colZeroFlag))
				{
					VecZeroFlag_L1 = 1;
				}
				*/
			}	
		}
		
		//****END(DIRECT PREDICTION)
		
		//*****************READ MOTION VECTORS**************************************************************
		int maxPartition, maxSubPartition;
		idx_tvl0 = 0;
		idx_tvl1 = 0;
		
		maxPartition = (slice_type == 1) ? BSpartQuant[mb_type]:partQuant[mb_type];
		for (part_idx=0; part_idx < maxPartition; part_idx++)
		{	
			mb_subtype = mb_subtypev[part_idx];
			if (mb_type)
        		maxSubPartition = (slice_type == 1) ? BSsubpartQuant[mb_subtype+1] : subpartQuant[mb_subtype+1];
        	else maxSubPartition = (slice_type == 1)?4:1;
   			for( sub_part_idx=0; sub_part_idx < maxSubPartition; sub_part_idx++)
			{				
	  			if (slice_type == 2 || intra)
	  			{
	  				pred_l0 = 0;
	  				pred_l1 = 0;
				}else if(slice_type == 0)
				{
	  				pred_l0 = 1;
	  				pred_l1 = 0;
				}else
				{
					//pred_l0 = 0;
	  				//pred_l1 = 0;
	  				if (mb_subtype == -1 && mb_type!=0)
	  				{
		  				pred_l0 = (MB_PRED[mb_type][part_idx]!=1)?1:0;
		  				pred_l1 = (MB_PRED[mb_type][part_idx]!=0)?1:0;
		  			}
		  			else if (mb_subtype == -2)
		  			{
			  			pred_l0 = Dir_pred_l0;
		  				pred_l1 = Dir_pred_l1;
			  		}
		  			else
		  			{
						if (BLOCK_PRED[mb_subtype+1]==0) //%erro
						{
		  					pred_l0 = 1;
		  					pred_l1 = 0;
						}
						if (BLOCK_PRED[mb_subtype+1]==1) //%erro
						{
		  					pred_l0 = 0;
		  					pred_l1 = 1;
						}
						if (BLOCK_PRED[mb_subtype+1]==2) //%erro
						{
		  					pred_l0 = 1;
		  					pred_l1 = 1;
						}
						if (BLOCK_PRED[mb_subtype+1]==-1) //%erro
						{
		  					pred_l0 = Dir_pred_l0;
		  					pred_l1 = Dir_pred_l1;
						}
						
						
	            	}
				}
				if ((mb_type && pred_l0 && !VecZeroFlag_L0 && slice_type == 0) ||
					(mb_subtype && pred_l0 && slice_type == 1))//(mb_subtype && pred_l0 && !VecZeroFlag_L0 && slice_type == 1))	
	            {
		         	//cout << "\nL0 mvd x y"; 
		         	in_temp_vec_l0();
		         	//cin >> trash >> temp_vec_l0[idx_tvl0][0]; cin >> trash >> temp_vec_l0[idx_tvl0++][1];
	            }
			}
		}

		maxPartition = (slice_type == 1) ? BSpartQuant[mb_type]:partQuant[mb_type];
		for (part_idx=0; part_idx < maxPartition; part_idx++)
		{	
			mb_subtype = mb_subtypev[part_idx];
        	if (mb_type)
        		maxSubPartition = (slice_type == 1) ? BSsubpartQuant[mb_subtype+1] : subpartQuant[mb_subtype+1];
        	else maxSubPartition = (slice_type == 1)?4:1;
   			for( sub_part_idx=0; sub_part_idx < maxSubPartition; sub_part_idx++)
			{				
	  			if (slice_type == 2 || intra)
	  			{
	  				pred_l0 = 0;
	  				pred_l1 = 0;
				}else if(slice_type == 0)
				{
	  				pred_l0 = 1;
	  				pred_l1 = 0;
				}else
				{
					//pred_l0 = 0;
	  				//pred_l1 = 0;
	  				if (mb_subtype == -1 && mb_type!=0)
	  				{
		  				pred_l0 = (MB_PRED[mb_type][part_idx]!=1)?1:0;
		  				pred_l1 = (MB_PRED[mb_type][part_idx]!=0)?1:0;
		  			}
		  			else
		  			{
						if (BLOCK_PRED[mb_subtype+1]==0) //%erro
						{
		  					pred_l0 = 1;
		  					pred_l1 = 0;
						}
						if (BLOCK_PRED[mb_subtype+1]==1) //%erro
						{
		  					pred_l0 = 0;
		  					pred_l1 = 1;
						}
						if (BLOCK_PRED[mb_subtype+1]==2) //%erro
						{
		  					pred_l0 = 1;
		  					pred_l1 = 1;
						}
						if (BLOCK_PRED[mb_subtype+1]==-1) //%erro
						{
		  					pred_l0 = Dir_pred_l0;
		  					pred_l1 = Dir_pred_l1;
						}
					}
				}
				
				if ((mb_type && pred_l1 && !VecZeroFlag_L1 && slice_type == 0) ||
					(((mb_type && mb_type!=22) || (mb_type == 22 && mb_subtype)) && pred_l1 && slice_type == 1))	//(((mb_type && mb_type!=22) || (mb_type == 22 && mb_subtype)) && pred_l1 && !VecZeroFlag_L1 && slice_type == 1))	
	            {
		        	//cout << "\nL1 mvd x y"; 
		        	in_temp_vec_l1();
		        	//cin >> trash >> temp_vec_l1[idx_tvl1][0]; cin >> trash >> temp_vec_l1[idx_tvl1++][1];
	            }
			}
		}		
				
		//*******************************************************************************
				
		idx_tvl0 = 0;
		idx_tvl1 = 0;		

		maxPartition = (slice_type == 1) ? BSpartQuant[mb_type]:partQuant[mb_type];
		for (part_idx=0; part_idx < maxPartition; part_idx++)
		{	
			mb_subtype = mb_subtypev[part_idx];
			if (slice_type == 0 && mb_type==2 && part_idx)
            {
              	ref_frame_l0 = ref_temp_l0[2]; 
              	ref_frame_l1 = ref_temp_l1[2]; 
        	}
            else if (slice_type == 0)
            {
            	ref_frame_l0 = ref_temp_l0[part_idx]; 
            	ref_frame_l1 = ref_temp_l1[part_idx]; 
       		}
       		
            if (slice_type == 1 && mb_type >= 4 && mb_type <= 21 && !(mb_type%2) && part_idx)
            {
	            ref_frame_l0 = ref_temp_l0[2]; 
	            ref_frame_l1 = ref_temp_l1[2]; 
            }
            else if (slice_type == 1 && (mb_type==0 || mb_subtype == 0))
            {
	            ref_frame_l0 = Dir_ref_frame_l0;
	            ref_frame_l1 = Dir_ref_frame_l1; 	            
	        }
            else if (slice_type == 1)
            {
            	ref_frame_l0 = ref_temp_l0[part_idx]; 	 	
            	ref_frame_l1 = ref_temp_l1[part_idx];
        	}           		
			
        	if (mb_type)
        		maxSubPartition = (slice_type == 1) ? BSsubpartQuant[mb_subtype+1] : subpartQuant[mb_subtype+1];
        	else maxSubPartition = (slice_type == 1)?4:1;
        	
			for( sub_part_idx=0; sub_part_idx < maxSubPartition; sub_part_idx++)
			{				
				
				//if (!((dir_pred && ((mb_type==0) || (mb_type==22 && !mb_subtype))) && !direct_spatial_mv_pred_flag)) //se não for temporal
				//{	
				
				// Cycle #1
				mb_x = 4*(block_x%4);
				mb_y = 4*(block_y%4);
				
				/*VecZeroFlag_L0 = 0;
        		VecZeroFlag_L1 = 0;
				*/
				if (slice_type == 1)
				{
					if (mb_subtype == -1)
					{
						blockshape_x = BSMBBLOCK_STEP [mb_type][0]*4;
	      				blockshape_y = BSMBBLOCK_STEP [mb_type][1]*4;
	  				}else
	  				{
		  				blockshape_x = BSBLOCK_STEP [mb_subtype][0]*4;
	      				blockshape_y = BSBLOCK_STEP [mb_subtype][1]*4;
		  			}
	  			}
	  			else /*if (slice_type == 0 || intra)*/
		  		{
		  			if (mb_subtype == -1)
					{
						blockshape_x = MBBLOCK_STEP [mb_type][0]*4;
	      				blockshape_y = MBBLOCK_STEP [mb_type][1]*4;
	  				}else
	  				{
		  				blockshape_x = BLOCK_STEP [mb_subtype][0]*4;
	      				blockshape_y = BLOCK_STEP [mb_subtype][1]*4;
		  			}
	  			}
	  			
	  			if (slice_type == 2 || intra)
	  			{
	  				pred_l0 = 0;
	  				pred_l1 = 0;
				}else if(slice_type == 0)
				{
	  				pred_l0 = 1;
	  				pred_l1 = 0;
				}else
				{
					//pred_l0 = 0;
	  				//pred_l1 = 0;
	  				if (mb_subtype == -1 && mb_type!=0)
	  				{
		  				pred_l0 = (MB_PRED[mb_type][part_idx]!=1)?1:0;
		  				pred_l1 = (MB_PRED[mb_type][part_idx]!=0)?1:0;
		  			}
		  			else
		  			{
						if (BLOCK_PRED[mb_subtype+1]==0) //%erro
						{
		  					pred_l0 = 1;
		  					pred_l1 = 0;
						}
						if (BLOCK_PRED[mb_subtype+1]==1) //%erro
						{
		  					pred_l0 = 0;
		  					pred_l1 = 1;
						}
						if (BLOCK_PRED[mb_subtype+1]==2) //%erro
						{
		  					pred_l0 = 1;
		  					pred_l1 = 1;
						}
						if (BLOCK_PRED[mb_subtype+1]==-1) //%erro
						{
		  					pred_l0 = Dir_pred_l0;
		  					pred_l1 = Dir_pred_l1;
						}
					}
				}
				
				mvPredType_l0 = MVPRED_MEDIAN;
	            mvPredType_l1 = MVPRED_MEDIAN;
				
	            if (!(dir_pred && ((mb_type==0) || (mb_type==22 && !mb_subtype))))
	            {	            
	            
					block_a_avail = (block_x != 0)?1:0;
					block_b_avail = (block_y != 0)?1:0; 
					block_c_avail = (block_b_avail && (block_x + blockshape_x/4 < max_x))?1:0;
					block_d_avail = (block_a_avail && block_b_avail)?1:0;
					
					
									
					// Cycle #2
					
					if (mb_y > 0)
					{
						if (mb_x < 8)  // first column of 8x8 blocks
		                {
			                if (mb_y==8)
			                {
				                if (blockshape_x == 16)      block_c_avail  = 0;
		                    	else                         block_c_avail &= 1;
		                  	}
		                  	else
		                  	{
		                    	if (mb_x+blockshape_x != 8)  block_c_avail &= 1;
		                    	else                         	block_c_avail  = 0;
		                  	}
		                }
		                else
		                {
		                  	if (mb_x+blockshape_x != 16) block_c_avail &= 1;
		                  	else                           	block_c_avail  = 0;
		                }
		            }
	            
	            
	
	            // Cycle #1
		            if (!(block_x%4))
		            {
			            idx_mv = (block_y%4);
			            idx_ref = idx_mv/2;
			            
			            block_a_ref_l0   = ref_a_l0[idx_ref];
		            	block_a_mv_l0[0] = mb_mv_a_l0[idx_mv][0];
		            	block_a_mv_l0[1] = mb_mv_a_l0[idx_mv][1];
		            	
		            	block_a_ref_l1   = ref_a_l1[idx_ref];
		            	block_a_mv_l1[0] = mb_mv_a_l1[idx_mv][0];
		            	block_a_mv_l1[1] = mb_mv_a_l1[idx_mv][1];
		            }
		            else
		            {
		                idx_mv = (block_y%4)*4+(block_x%4 - 1);
		                idx_ref =((block_y%4)/2)*2+(block_x%4 - 1)/2;
		                
		                block_a_ref_l0   = mb_ref_l0[idx_ref]; 
		            	block_a_mv_l0[0] = mb_mv_l0[idx_mv][0];
		            	block_a_mv_l0[1] = mb_mv_l0[idx_mv][1];
		            	
		            	block_a_ref_l1   = mb_ref_l1[idx_ref]; 
		            	block_a_mv_l1[0] = mb_mv_l1[idx_mv][0];
		            	block_a_mv_l1[1] = mb_mv_l1[idx_mv][1];
		               
		            }
		            // Cycle #2
		    
		                        
		            
		            if (!(block_y%4)) 
		            {
			            idx_mv = block_x%4;
			            idx_ref =(block_x%4)/2;
			            
			            block_b_ref_l0   = ref_b_l0[idx_ref];
		            	block_b_mv_l0[0] = mb_mv_b_l0[idx_mv][0];
			        	block_b_mv_l0[1] = mb_mv_b_l0[idx_mv][1];
			        	
			        	block_b_ref_l1   = ref_b_l1[idx_ref];
		            	block_b_mv_l1[0] = mb_mv_b_l1[idx_mv][0];
			        	block_b_mv_l1[1] = mb_mv_b_l1[idx_mv][1];
			            
		            }else
		            {
			            idx_mv = (block_y%4 -1)*4+(block_x%4);
			            idx_ref =((block_y%4 -1)/2)*2+(block_x%4)/2;
			            
			            block_b_ref_l0   = mb_ref_l0[idx_ref]; 
		            	block_b_mv_l0[0] = mb_mv_l0[idx_mv][0];
			        	block_b_mv_l0[1] = mb_mv_l0[idx_mv][1];
			        	
			        	block_b_ref_l1   = mb_ref_l1[idx_ref]; 
		            	block_b_mv_l1[0] = mb_mv_l1[idx_mv][0];
			        	block_b_mv_l1[1] = mb_mv_l1[idx_mv][1];
		            }
		            
		            // Cycle #3  
			                      
		            
		            if (!(block_y%4) || !block_c_avail ) 
		            {
			            if (block_x%4 + blockshape_x/4 > 3){
				            block_c_ref_l0 = ref_c_l0;
				        	block_c_mv_l0[0] = mb_mv_c_l0[0];
				        	block_c_mv_l0[1] = mb_mv_c_l0[1];
				        	
				        	block_c_ref_l1 = ref_c_l1;
				        	block_c_mv_l1[0] = mb_mv_c_l1[0];
				        	block_c_mv_l1[1] = mb_mv_c_l1[1];
			        	}
			        	else
			        	{
				        	idx_mv = block_x%4 + blockshape_x/4;
			            	idx_ref =(block_x%4 + blockshape_x/4)/2;
			            	
			            	block_c_ref_l0   = ref_b_l0[idx_ref];
		            		block_c_mv_l0[0] = mb_mv_b_l0[idx_mv][0];
			        		block_c_mv_l0[1] = mb_mv_b_l0[idx_mv][1];
				        	
			        		block_c_ref_l1   = ref_b_l1[idx_ref];
		            		block_c_mv_l1[0] = mb_mv_b_l1[idx_mv][0];
			        		block_c_mv_l1[1] = mb_mv_b_l1[idx_mv][1];
			        	}
			            
			        }else
			        {
				        idx_mv = (block_y%4 -1)*4 + block_x%4 + blockshape_x/4;
				        idx_ref = ((block_y%4 -1)/2)*2 + (block_x%4 + blockshape_x/4)/2;
				        
				        block_c_ref_l0   = mb_ref_l0[idx_ref]; 
		            	block_c_mv_l0[0] = mb_mv_l0[idx_mv][0];
			        	block_c_mv_l0[1] = mb_mv_l0[idx_mv][1]; 
			        	
			        	block_c_ref_l1   = mb_ref_l1[idx_ref]; 
		            	block_c_mv_l1[0] = mb_mv_l1[idx_mv][0];
			        	block_c_mv_l1[1] = mb_mv_l1[idx_mv][1]; 
			        }
			        
			        // Cycle #4    
		            
		            if (!(block_y%4) && !(block_x%4))  
		            {
			            block_d_ref_l0 = ref_d_l0;
			        	block_d_mv_l0[0] = mb_mv_d_l0[0];
			        	block_d_mv_l0[1] = mb_mv_d_l0[1]; 
			        	
			        	block_d_ref_l1 = ref_d_l1;
			        	block_d_mv_l1[0] = mb_mv_d_l1[0];
			        	block_d_mv_l1[1] = mb_mv_d_l1[1]; 
					}else
					{
						if (!(block_x%4))
						{
							idx_mv = block_y%4 -1;
							idx_ref = idx_mv/2;
	
							block_d_ref_l0   = ref_a_l0[idx_ref];
		            		block_d_mv_l0[0] = mb_mv_a_l0[idx_mv][0];
			        		block_d_mv_l0[1] = mb_mv_a_l0[idx_mv][1];
			        		
			        		block_d_ref_l1   = ref_a_l1[idx_ref];
		            		block_d_mv_l1[0] = mb_mv_a_l1[idx_mv][0];
			        		block_d_mv_l1[1] = mb_mv_a_l1[idx_mv][1];
			        		
						}else if(!(block_y%4))
						{
							idx_mv = block_x%4 -1;
							idx_ref = idx_mv/2;
							
							block_d_ref_l0   = ref_b_l0[idx_ref];
		            		block_d_mv_l0[0] = mb_mv_b_l0[idx_mv][0];
			        		block_d_mv_l0[1] = mb_mv_b_l0[idx_mv][1];
			        		
			        		block_d_ref_l1   = ref_b_l1[idx_ref];
		            		block_d_mv_l1[0] = mb_mv_b_l1[idx_mv][0];
			        		block_d_mv_l1[1] = mb_mv_b_l1[idx_mv][1];
			        		
						}else
						{
							idx_mv = (block_y%4 -1)*4 + block_x%4 - 1;
							idx_ref = ((block_y%4 -1)/2)*2 + (block_x%4 - 1)/2;
							
							block_d_ref_l0   = mb_ref_l0[idx_ref];
		            		block_d_mv_l0[0] = mb_mv_l0[idx_mv][0];
			        		block_d_mv_l0[1] = mb_mv_l0[idx_mv][1]; 
			        		
			        		block_d_ref_l1   = mb_ref_l1[idx_ref];
		            		block_d_mv_l1[0] = mb_mv_l1[idx_mv][0];
			        		block_d_mv_l1[1] = mb_mv_l1[idx_mv][1]; 
						}
					}
				}
				else if (direct_spatial_mv_pred_flag)
				{					
					//cout << "aha";
					block_a_avail = ((block_x - block_x%4)!= 0)?1:0;
					block_b_avail = ((block_y - block_y%4) != 0)?1:0; 
					block_c_avail = (block_b_avail && ((block_x - block_x%4) + 4 < max_x))?1:0;
					block_d_avail = (block_a_avail && block_b_avail)?1:0;
					
					idx = block_y * 36 + block_x;
					idx_ref = ((block_y - block_y%2)/2)* 18 + (block_x - block_x%2)/2;
					
		    		if (direct_8x8_inference_flag)
		    		{
			    		mb_mvcol_l0[0] = mv_colocated_l0[RSD(block_y)][RSD(block_x)][0];
						mb_mvcol_l0[1] = mv_colocated_l0[RSD(block_y)][RSD(block_x)][1];
						mb_refcol_l0   = ref_colocated_l0[RSD(block_y)][RSD(block_x)];
							
						mb_mvcol_l1[0] = mv_colocated_l1[RSD(block_y)][RSD(block_x)][0];
						mb_mvcol_l1[1] = mv_colocated_l1[RSD(block_y)][RSD(block_x)][1];
						mb_refcol_l1   = ref_colocated_l1[RSD(block_y)][RSD(block_x)];
			    	}else
			    	{
			    		mb_mvcol_l0[0] = mv_colocated_l0[block_y][block_x][0];
						mb_mvcol_l0[1] = mv_colocated_l0[block_y][block_x][1];
						mb_refcol_l0   = ref_colocated_l0[block_y][block_x];
							
						mb_mvcol_l1[0] = mv_colocated_l1[block_y][block_x][0];
						mb_mvcol_l1[1] = mv_colocated_l1[block_y][block_x][1];
						mb_refcol_l1   = ref_colocated_l1[block_y][block_x];
			    	}
				    //colZeroFlag = (short_term && !mb_refcol_l0 && ((mb_mvcol_l0[0] >= -1 && mb_mvcol_l0[0] <= 1) && (mb_mvcol_l0[1] >= -1 && mb_mvcol_l0[1] <= 1)))?1:0;				
				    
				    colZeroFlag = (short_term 
				    	&& ((!mb_refcol_l0 && (mb_mvcol_l0[0] >= -1 && mb_mvcol_l0[0] <= 1) && (mb_mvcol_l0[1] >= -1 && mb_mvcol_l0[1] <= 1))
			    				||(mb_refcol_l0 == -1  && !mb_refcol_l1 && (mb_mvcol_l1[0] >= -1 && mb_mvcol_l1[0] <= 1) && (mb_mvcol_l1[1] >= -1 && mb_mvcol_l1[1] <= 1))))?1:0;
			    				
			    	/*p->moving_block[j][i] = 
              !((!p->is_long_term 
              && ((p->ref_idx[LIST_0][j][i] == 0) 
              &&  (abs(p->mv[LIST_0][j][i][0])>>1 == 0) 
              &&  (abs(p->mv[LIST_0][j][i][1])>>1 == 0))) 
              || ((p->ref_idx[LIST_0][j][i] == -1) 
              &&  (p->ref_idx[LIST_1][j][i] == 0) 
              &&  (abs(p->mv[LIST_1][j][i][0])>>1 == 0) 
              &&  (abs(p->mv[LIST_1][j][i][1])>>1 == 0)));			
			    */
				    			    
					if (directZeroPredictionFlag || Dir_ref_frame_l0 < 0 || (Dir_ref_frame_l0 ==0 && colZeroFlag))
					{
						VecZeroFlag_L0 = 1;	
					}
					else VecZeroFlag_L0 = 0;	
					if (directZeroPredictionFlag || Dir_ref_frame_l1 < 0 || (Dir_ref_frame_l1 ==0 && colZeroFlag))
					{
						VecZeroFlag_L1 = 1;
					}
					else VecZeroFlag_L1 = 0;
						
					idx_mv = 0;
			        idx_ref = 0;
			            
			        block_a_ref_l0   = ref_a_l0[idx_ref];
		            block_a_mv_l0[0] = mb_mv_a_l0[idx_mv][0];
		            block_a_mv_l0[1] = mb_mv_a_l0[idx_mv][1];
		            	
		            block_a_ref_l1   = ref_a_l1[idx_ref];
		            block_a_mv_l1[0] = mb_mv_a_l1[idx_mv][0];
		            block_a_mv_l1[1] = mb_mv_a_l1[idx_mv][1];
		            
		            block_b_ref_l0   = ref_b_l0[idx_ref];
		            block_b_mv_l0[0] = mb_mv_b_l0[idx_mv][0];
			        block_b_mv_l0[1] = mb_mv_b_l0[idx_mv][1];
			        	
			        block_b_ref_l1   = ref_b_l1[idx_ref];
		            block_b_mv_l1[0] = mb_mv_b_l1[idx_mv][0];
			        block_b_mv_l1[1] = mb_mv_b_l1[idx_mv][1];
			        
			        block_c_ref_l0 = ref_c_l0;
				    block_c_mv_l0[0] = mb_mv_c_l0[0];
				    block_c_mv_l0[1] = mb_mv_c_l0[1];
				        	
				    block_c_ref_l1 = ref_c_l1;
				    block_c_mv_l1[0] = mb_mv_c_l1[0];
				    block_c_mv_l1[1] = mb_mv_c_l1[1];
				    
				    block_d_ref_l0 = ref_d_l0;
			        block_d_mv_l0[0] = mb_mv_d_l0[0];
			        block_d_mv_l0[1] = mb_mv_d_l0[1]; 
			        	
			        block_d_ref_l1 = ref_d_l1;
			        block_d_mv_l1[0] = mb_mv_d_l1[0];
			        block_d_mv_l1[1] = mb_mv_d_l1[1]; 
					
				}
				else		//direta temporal
				{
         			int ref_idx =  0;    
         			
         			if (direct_8x8_inference_flag)
		    			{
							mb_refcol_l0   = ref_colocated_l0[RSD(block_y)][RSD(block_x)];
							mb_refcol_l1   = ref_colocated_l1[RSD(block_y)][RSD(block_x)];
			    		}else
			    		{
				    		mb_refcol_l0   = ref_colocated_l0[block_y][block_x];
							mb_refcol_l1   = ref_colocated_l1[block_y][block_x];
			    		}
          			
				/*	mb_refcol_l0   = ref_colocated_l0[block_y][block_x];
					mb_refcol_l1   = ref_colocated_l1[block_y][block_x];
					*/
					if (mb_refcol_l0 == -1) ref_idx = mb_refcol_l1;
					else ref_idx = mb_refcol_l0;
					
					if (ref_idx == -1){
						
						ref_frame_l0 = 0;
						ref_frame_l1 = 0;
						vec_x_l0=0;           
	            		vec_y_l0=0;
						vec_x_l1=0;           
	            		vec_y_l1=0;
					}
					else {
						
						if (sub_part_idx == 0)    {
							int mapped_index = 0; 
							int CurrPOC = 0;
							int POC0= 0;
							int POC1= 0;
							
							if (!(icounter%3)) CurrPOC = icounter + 3;
							else CurrPOC = icounter;
							
							mapped_index = ref_idx;// + 1;
							
							
							// ler mapped_index
							
							if (icounter%3 == 1){
							 POC1 = icounter + 2;
							 if (mapped_index == 0) 		POC0 = ((int)icounter/3) * 3;
							 else if (mapped_index == 1) POC0 = ((int)icounter/3) * 3 - 3;
							 else if (mapped_index == 2) POC0 = ((int)icounter/3) * 3 - 6;
							 else POC0 = ((int)icounter/3) * 3 - 9;						 
							 
					 		}
							else {
							 POC1= icounter + 1;
							 if (mapped_index == 0) 		POC0 = ((int)icounter/3) * 3;
							 else if (mapped_index == 1) POC0 = ((int)icounter/3) * 3 - 3;
							 else if (mapped_index == 2) POC0 = ((int)icounter/3) * 3 - 6;
							 else POC0 = ((int)icounter/3) * 3 - 9;	
							}
							
							
							
							ref_frame_l0 = mapped_index;
							ref_frame_l1 = 0;
							
						/*	mb_mvcol_l0[0] = mv_colocated_l0[block_y][block_x][0];
							mb_mvcol_l0[1] = mv_colocated_l0[block_y][block_x][1];
          					mb_mvcol_l1[0] = mv_colocated_l1[block_y][block_x][0];
							mb_mvcol_l1[1] = mv_colocated_l1[block_y][block_x][1];*/
					    	
							//bloco de predição	
							int DiffPOC_D = POC1 - POC0;
      						int DiffPOC_B = CurrPOC - POC0;
      						//cout << "curPOC:"<< CurrPOC;
      						//cout << "POC0:"<< POC0;
      						//cout << "POC1:"<< POC1;
                        	
        					int TDD = clip(-128,127,DiffPOC_D);
        					int TDB = clip(-128,127,DiffPOC_B);         
        					
        					
        					int X = (16384 + (TDD >> 1)) / TDD;		
        					DistScaleFactor = clip(-1024, 1023, ((TDB * X + 32) >> 6));  
    					}	//bloco de predição	
        				
        				//cout << "dist"<< DistScaleFactor;
        				
        				if (direct_8x8_inference_flag)
		    			{
			    			mb_mvcol_l0[0] = mv_colocated_l0[RSD(block_y)][RSD(block_x)][0];
							mb_mvcol_l0[1] = mv_colocated_l0[RSD(block_y)][RSD(block_x)][1];						
							
							mb_mvcol_l1[0] = mv_colocated_l1[RSD(block_y)][RSD(block_x)][0];
							mb_mvcol_l1[1] = mv_colocated_l1[RSD(block_y)][RSD(block_x)][1];							
			    		}else
			    		{
			    			mb_mvcol_l0[0] = mv_colocated_l0[block_y][block_x][0];
							mb_mvcol_l0[1] = mv_colocated_l0[block_y][block_x][1];
							
							mb_mvcol_l1[0] = mv_colocated_l1[block_y][block_x][0];
							mb_mvcol_l1[1] = mv_colocated_l1[block_y][block_x][1];
			    		}
			    		
			    		//cout << "colocated" << mb_mvcol_l0[0];
						
						vec_x_l0= (int)((DistScaleFactor * mb_mvcol_l0[0] + 128 ) >> 8);           
	            		vec_y_l0= (int)((DistScaleFactor * mb_mvcol_l0[1] + 128 ) >> 8);
						vec_x_l1= vec_x_l0 - mb_mvcol_l0[0];           
	            		vec_y_l1= vec_y_l0 - mb_mvcol_l0[1];
	            		//cout << "vec x" << vec_x_l0; cout << "vec y" << vec_y_l0;
					}
					
										
				}
				// Cycle #5
				
				if ((!(dir_pred && ((mb_type==0) || (mb_type==22 && !mb_subtype)))) || 
					(direct_spatial_mv_pred_flag)) //se não for temporal
				{   
					         
	            // Cycle #6	            
	            mv_a_l0[0] = block_a_avail  ? block_a_mv_l0[0] : 0;
	            mv_b_l0[0] = block_b_avail  ? block_b_mv_l0[0] : 0;
	            mv_c_l0[0] = block_c_avail  ? block_c_mv_l0[0] : block_d_avail? block_d_mv_l0[0]:0;
	            
	            mv_a_l1[0] = block_a_avail  ? block_a_mv_l1[0] : 0;
	            mv_b_l1[0] = block_b_avail  ? block_b_mv_l1[0] : 0;
	            mv_c_l1[0] = block_c_avail  ? block_c_mv_l1[0] : block_d_avail? block_d_mv_l1[0]:0;
	
	            mv_a_l0[1] = block_a_avail  ? block_a_mv_l0[1] : 0;
	            mv_b_l0[1] = block_b_avail  ? block_b_mv_l0[1] : 0;
	            mv_c_l0[1] = block_c_avail  ? block_c_mv_l0[1] : block_d_avail? block_d_mv_l0[1]:0;
	
				mv_a_l1[1] = block_a_avail  ? block_a_mv_l1[1] : 0;
	            mv_b_l1[1] = block_b_avail  ? block_b_mv_l1[1] : 0;
	            mv_c_l1[1] = block_c_avail  ? block_c_mv_l1[1] : block_d_avail? block_d_mv_l1[1]:0;

	            rFrameL_l0    = block_a_avail    ? block_a_ref_l0 : -1;
	            rFrameU_l0    = block_b_avail    ? block_b_ref_l0 : -1;
	            rFrameUR_l0   = block_c_avail    ? block_c_ref_l0 : block_d_avail ? block_d_ref_l0 : -1;
	            
	            rFrameL_l1    = block_a_avail    ? block_a_ref_l1 : -1;
	            rFrameU_l1    = block_b_avail    ? block_b_ref_l1 : -1;
	            rFrameUR_l1   = block_c_avail    ? block_c_ref_l1 : block_d_avail ? block_d_ref_l1 : -1;
				
	            // Cycle #7
	            if(rFrameL_l0 == ref_frame_l0 && rFrameU_l0 != ref_frame_l0 && rFrameUR_l0 != ref_frame_l0) mvPredType_l0 = MVPRED_L;
	            else if(rFrameL_l0 != ref_frame_l0 && rFrameU_l0 == ref_frame_l0 && rFrameUR_l0 != ref_frame_l0)  mvPredType_l0 = MVPRED_U;
	            else if(rFrameL_l0 != ref_frame_l0 && rFrameU_l0 != ref_frame_l0 && rFrameUR_l0 == ref_frame_l0)  mvPredType_l0 = MVPRED_UR;
	            
	            if(rFrameL_l1 == ref_frame_l1 && rFrameU_l1 != ref_frame_l1 && rFrameUR_l1 != ref_frame_l1) mvPredType_l1 = MVPRED_L;
	            else if(rFrameL_l1 != ref_frame_l1 && rFrameU_l1 == ref_frame_l1 && rFrameUR_l1 != ref_frame_l1)  mvPredType_l1 = MVPRED_U;
	            else if(rFrameL_l1 != ref_frame_l1 && rFrameU_l1 != ref_frame_l1 && rFrameUR_l1 == ref_frame_l1)  mvPredType_l1 = MVPRED_UR;
	            
	            // Cycle #8            
	            if(blockshape_x == 8 && blockshape_y == 16)
	            {
		            if(mb_x == 0)
		            {
			            if(rFrameL_l0 == ref_frame_l0) mvPredType_l0 = MVPRED_L;
			            if(rFrameL_l1 == ref_frame_l1) mvPredType_l1 = MVPRED_L;
			        }else
	              	{
		              	if( rFrameUR_l0 == ref_frame_l0) mvPredType_l0 = MVPRED_UR;
		              	if( rFrameUR_l1 == ref_frame_l1) mvPredType_l1 = MVPRED_UR;
	              	}
	            }
	            else if(blockshape_x == 16 && blockshape_y == 8)
	            {
		            if(mb_y == 0)
		            {                
			            if(rFrameU_l0 == ref_frame_l0) mvPredType_l0 = MVPRED_U;
			            if(rFrameU_l1 == ref_frame_l1) mvPredType_l1 = MVPRED_U;
			        }else
			        {
				        if(rFrameL_l0 == ref_frame_l0) mvPredType_l0 = MVPRED_L;
				        if(rFrameL_l1 == ref_frame_l1) mvPredType_l1 = MVPRED_L;
				    }
	            }
	            
	            int zeroMotionLeft  = !block_a_avail ? 1 : rFrameL_l0==0 && mv_a_l0[0]==0 && mv_a_l0[1]==0 ? 1 : 0;
    			int zeroMotionAbove = !block_b_avail ? 1 : rFrameU_l0==0 && mv_b_l0[0]==0 && mv_b_l0[1]==0 ? 1 : 0;
				
    			// Cycle #9
	            switch (mvPredType_l0)
	            {
		            case MVPRED_MEDIAN:
		            	if(!(block_b_avail || block_c_avail || block_d_avail))
		            	{
	            			pred_vec_l0[0] = mv_a_l0[0];
	            			pred_vec_l0[1] = mv_a_l0[1];
                		}else{
	            			pred_vec_l0[0] = mv_a_l0[0]+mv_b_l0[0]+mv_c_l0[0]-min(mv_a_l0[0],min(mv_b_l0[0],mv_c_l0[0]))-max(mv_a_l0[0],max(mv_b_l0[0],mv_c_l0[0]));
	            			pred_vec_l0[1] = mv_a_l0[1]+mv_b_l0[1]+mv_c_l0[1]-min(mv_a_l0[1],min(mv_b_l0[1],mv_c_l0[1]))-max(mv_a_l0[1],max(mv_b_l0[1],mv_c_l0[1]));
            			}
	                break;
	              	case MVPRED_L:
	                	pred_vec_l0[0] = mv_a_l0[0];
	                	pred_vec_l0[1] = mv_a_l0[1];
	                break;
	              	case MVPRED_U:
	                	pred_vec_l0[0] = mv_b_l0[0];
	                	pred_vec_l0[1] = mv_b_l0[1];
	                break;
	              	case MVPRED_UR:
	                	pred_vec_l0[0] = mv_c_l0[0];
	                	pred_vec_l0[1] = mv_c_l0[1];
	                break;
	              default:
	                break;
	            }
	            
	            switch (mvPredType_l1)
	            {
		            case MVPRED_MEDIAN:
		            	if(!(block_b_avail || block_c_avail || block_d_avail))
		            	{
	            			pred_vec_l1[0] = mv_a_l1[0];
	            			pred_vec_l1[1] = mv_a_l1[1];
                		}else{
	            			pred_vec_l1[0] = mv_a_l1[0]+mv_b_l1[0]+mv_c_l1[0]-min(mv_a_l1[0],min(mv_b_l1[0],mv_c_l1[0]))-max(mv_a_l1[0],max(mv_b_l1[0],mv_c_l1[0]));
	            			pred_vec_l1[1] = mv_a_l1[1]+mv_b_l1[1]+mv_c_l1[1]-min(mv_a_l1[1],min(mv_b_l1[1],mv_c_l1[1]))-max(mv_a_l1[1],max(mv_b_l1[1],mv_c_l1[1]));
            			}
	                break;
	              	case MVPRED_L:
	                	pred_vec_l1[0] = mv_a_l1[0];
	                	pred_vec_l1[1] = mv_a_l1[1];
	                break;
	              	case MVPRED_U:
	                	pred_vec_l1[0] = mv_b_l1[0];
	                	pred_vec_l1[1] = mv_b_l1[1];
	                break;
	              	case MVPRED_UR:
	                	pred_vec_l1[0] = mv_c_l1[0];
	                	pred_vec_l1[1] = mv_c_l1[1];
	                break;
	              default:
	                break;
	            }
	            
	            // Cycle #10
	            
	            if ((zeroMotionAbove || zeroMotionLeft) && slice_type == 0 && !mb_type)
		        {
			            pred_vec_l0[0] = 0;
			            pred_vec_l0[1] = 0;
		        }
	            
	            /*if ((!part_idx && !sub_part_idx && !mb_type) || (mb_type == 22 && !mb_subtype && !sub_part_idx))
	            {
		            Dir_pred_vec_l0[0] = pred_vec_l0[0];
		            Dir_pred_vec_l0[1] = pred_vec_l0[1];
		            Dir_pred_vec_l1[0] = pred_vec_l1[0];
		            Dir_pred_vec_l1[1] = pred_vec_l1[1];
		        }*/
	            
	            
	            if ((mb_type && pred_l0 && !VecZeroFlag_L0 && slice_type == 0) ||
					(mb_subtype && pred_l0 && slice_type == 1))//(mb_subtype && pred_l0 && !VecZeroFlag_L0 && slice_type == 1))	
        		{
            		mvd_x_l0 =  temp_vec_l0[idx_tvl0][0];//read l0
            		mvd_y_l0 =  temp_vec_l0[idx_tvl0++][1];//read l0
        		}
        		else
        		{
            		mvd_x_l0 = 0; 
            		mvd_y_l0 = 0; 
		        }
		        
		        /*if ((!mb_type || (mb_type == 22 && !mb_subtype)) && slice_type == 1)
		        {
			        pred_vec_l0[0] = Dir_pred_vec_l0[0];
			        pred_vec_l0[1] = Dir_pred_vec_l0[1];
			    }*/
								
	            if (pred_l0 && (!VecZeroFlag_L0 || (mb_type == 22 && mb_subtype))) //(!mb_type || (mb_type == 22 && !mb_subtype)))// &&
		        {
	            	vec_x_l0=mvd_x_l0+pred_vec_l0[0];           
	            	vec_y_l0=mvd_y_l0+pred_vec_l0[1];           
            	}
            	else
            	{
	            	vec_x_l0=0;           
	            	vec_y_l0=0;
	            	if (!VecZeroFlag_L0)
	            		ref_frame_l0 = -1;
	            }
   
		       if ((mb_type && pred_l1 && !VecZeroFlag_L1 && slice_type == 0) ||
					(((mb_type && mb_type!=22) || (mb_type == 22 && mb_subtype)) && pred_l1 && slice_type == 1))	//(((mb_type && mb_type!=22) || (mb_type == 22 && mb_subtype)) && pred_l1 && !VecZeroFlag_L1 && slice_type == 1))	
        		{
            		mvd_x_l1 =  temp_vec_l1[idx_tvl1][0];//read L0
            		mvd_y_l1 =  temp_vec_l1[idx_tvl1++][1];//read L0
        		}
        		else
        		{
            		mvd_x_l1 = 0; 
            		mvd_y_l1 = 0; 
		        }
				
		       /* if ((!mb_type || (mb_type == 22 && !mb_subtype)) && slice_type == 1)
		        {
			        pred_vec_l1[0] = Dir_pred_vec_l1[0];
			        pred_vec_l1[1] = Dir_pred_vec_l1[1];
			    }*/
		        
	            if (pred_l1 && (!VecZeroFlag_L1 || (mb_type == 22 && mb_subtype)))//&& 
		        {
	            	vec_x_l1=mvd_x_l1+pred_vec_l1[0];           
	            	vec_y_l1=mvd_y_l1+pred_vec_l1[1];           
            	}
            	else
            	{
	            	vec_x_l1=0;           
	            	vec_y_l1=0;
	            	if (!VecZeroFlag_L1)
	            		ref_frame_l1 = -1;
	            }          
	            
	             
            }           	 
                     
            
            if (intra) {
	            ref_frame_l0 = -1;
	            ref_frame_l1 = -1;
	            vec_x_l0=0;           
	            vec_y_l0=0;
	            vec_x_l1=0;           
	            vec_y_l1=0;
            }
            
                 
				//cout << "\n[" << block_y <<","<<block_x<<"]["<< block_a_avail <<"|"<< block_b_avail <<"|" << block_c_avail <<"|" << block_d_avail << "] M=" << mvPredType_l0 <<" -> a=" << rFrameL_l0 << "[" << mv_a_l0[0] <<","<<mv_a_l0[1]<<"] -> b=" << rFrameU_l0 << "[" << mv_b_l0[0] <<","<<mv_b_l0[1]<<"] -> c=" << rFrameUR_l0 << "["<< mv_c_l0[0] <<","<<mv_c_l0[1]  
				//		<<"]{" << VecZeroFlag_L0 << "}  M=" << mvPredType_l0 <<" -> a=" << rFrameL_l1 << "[" << mv_a_l1[0] <<","<<mv_a_l1[1]<<"] -> b=" << rFrameU_l1 << "[" << mv_b_l1[0] <<","<<mv_b_l1[1]<<"] -> c=" << rFrameUR_l1 << "["<< mv_c_l1[0] <<","<<mv_c_l1[1]<<"] {" << VecZeroFlag_L1 << "}";        
    
	            
				//int idx = mbp_pos(mb_type, mb_subtype, part_idx, sub_part_idx);
				//idx = Block_Order[idx];
				for (int j=block_y%4; j <= block_y%4+(blockshape_y -1)/4; j++)
					for (int i=block_x%4; i <= block_x%4+(blockshape_x -1)/4; i++)
					{
						idx = j*4 + i;
						idx_ref = (j/2)*2 + i/2;
						
						mb_mv_l0[idx][0] = vec_x_l0;
						mb_mv_l0[idx][1] = vec_y_l0;
						mb_ref_l0[idx_ref] = ref_frame_l0;
						
						mb_mv_l1[idx][0] = vec_x_l1;
						mb_mv_l1[idx][1] = vec_y_l1;
						mb_ref_l1[idx_ref] = ref_frame_l1;
					}
				
				int sx, sy;
				
				bp_pos(slice_type, mb_subtype, sub_part_idx, &sx, &sy);
				
				block_x = block_x - block_x%2 + sx;
      			block_y = block_y - block_y%2 + sy;
      		}
			int x,y;
			mbp_pos(slice_type, mb_type, part_idx, &x, &y);
			//block_x = block_x - block_x%4 + x;
      		//block_y = block_y - block_y%4 + y;
      		block_x = rBX + x;
      		block_y = rBY + y;
		}
		//Atualizar Memoria!!!!!!!!!!!!!!!!!
		block_x = rBX;
	    block_y = rBY;
		
	    int block_x_ant = block_x;
	    
	    if (slice_type != 1)
		for (int j=0; j < 4; j++)
			for (int i=0; i < 4; i++)
			{
				idx = (block_y + j) * 36 + block_x + i;
				idx_ref = ((block_y + j - block_y%2 -j%2)/2) * 18 + (block_x + i - block_x%2 - i%2)/2;
				idx_mv = j*4 + i;
				int idx_mb_ref = (j/2)*2 + i/2;
				
				mv_colocated_l0[block_y + j][block_x + i][0] = mb_mv_l0[idx_mv][0];
				mv_colocated_l0[block_y + j][block_x + i][1] = mb_mv_l0[idx_mv][1];
				ref_colocated_l0[block_y+ j][block_x + i] = mb_ref_l0[idx_mb_ref];
				
				mv_colocated_l1[block_y + j][block_x + i][0] = mb_mv_l1[idx_mv][0];
				mv_colocated_l1[block_y + j][block_x + i][1] = mb_mv_l1[idx_mv][1];
				ref_colocated_l1[block_y+ j][block_x + i] = mb_ref_l1[idx_mb_ref];
			}
	    
	    
	    block_x += 4;
	    if (block_x == max_x) 
	    {
		    block_x = 0;
		    block_y +=4;
		    if (block_y == max_y) block_y = 0; 
		}
	    	    		
		mb_mv_d_l0[0] = mb_mv_b_l0[3][0];
		mb_mv_d_l0[1] = mb_mv_b_l0[3][1];
		ref_d_l0      = ref_b_l0[1];
		
		mb_mv_d_l1[0] = mb_mv_b_l1[3][0];
		mb_mv_d_l1[1] = mb_mv_b_l1[3][1];
		ref_d_l1      = ref_b_l1[1];
		
		mb_mv_c_l0[0] = mv_memory_x_l0[block_x+4];
		mb_mv_c_l0[1] = mv_memory_y_l0[block_x+4];
		ref_c_l0      = ref_memory_l0[(block_x+4)/2];
			
		mb_mv_c_l1[0] = mv_memory_x_l1[block_x+4];
		mb_mv_c_l1[1] = mv_memory_y_l1[block_x+4];
		ref_c_l1      = ref_memory_l1[(block_x+4)/2];
		
		for (int ii=0;ii < 4; ii++)
		{
			mb_mv_a_l0[ii][0] = mb_mv_l0[ii*4+3][0];
			mb_mv_a_l0[ii][1] = mb_mv_l0[ii*4+3][1];
			mb_mv_b_l0[ii][0] = mv_memory_x_l0[block_x+ii];
			mb_mv_b_l0[ii][1] = mv_memory_y_l0[block_x+ii];
			mv_memory_x_l0[block_x_ant+ii] = mb_mv_l0[12+ii][0];
			mv_memory_y_l0[block_x_ant+ii] = mb_mv_l0[12+ii][1];
			
			mb_mv_a_l1[ii][0] = mb_mv_l1[ii*4+3][0];
			mb_mv_a_l1[ii][1] = mb_mv_l1[ii*4+3][1];
			mb_mv_b_l1[ii][0] = mv_memory_x_l1[block_x+ii];
			mb_mv_b_l1[ii][1] = mv_memory_y_l1[block_x+ii];
			mv_memory_x_l1[block_x_ant+ii] = mb_mv_l1[12+ii][0];
			mv_memory_y_l1[block_x_ant+ii] = mb_mv_l1[12+ii][1];
		}
				
		ref_a_l0[0] = mb_ref_l0[1];
		ref_a_l0[1] = mb_ref_l0[3];
		ref_b_l0[0] = ref_memory_l0[(block_x+0)/2];
		ref_b_l0[1] = ref_memory_l0[(block_x+2)/2];
		ref_memory_l0[(block_x_ant+0)/2] = mb_ref_l0[2];
		ref_memory_l0[(block_x_ant+2)/2] = mb_ref_l0[3];
		
		ref_a_l1[0] = mb_ref_l1[1];
		ref_a_l1[1] = mb_ref_l1[3];
		ref_b_l1[0] = ref_memory_l1[(block_x+0)/2];
		ref_b_l1[1] = ref_memory_l1[(block_x+2)/2];
		ref_memory_l1[(block_x_ant+0)/2] = mb_ref_l1[2];
		ref_memory_l1[(block_x_ant+2)/2] = mb_ref_l1[3];
		
		//*******************************************************************************
		int erroM = 0, erroR = 0;
		
		in_trash();
		//cin >> trash;
		for(int ii=0;ii<4;ii++)
			read_ref_l0(ii);
		    //cin >> rd_ref_l0[ii];                
		in_trash();
		//cin >> trash;    
        for(int ii=0;ii<4;ii++)
        	read_ref_l1(ii);
            //cin >> rd_ref_l1[ii];                
               
        for(int ii=0;ii<16;ii++){
            if (!(ii%4)) in_trash(); //cin >> trash;
            read_mv_l0(ii);
            //cin >> rd_mv_l0[ii][0] >> rd_mv_l0[ii][1];                
            }
		for(int ii=0;ii<16;ii++){
            if (!(ii%4)) in_trash();  //cin >> trash;
            read_mv_l1(ii);
            //cin >> rd_mv_l1[ii][0] >> rd_mv_l1[ii][1];                
            }
    	
		//cout << "\nVetores";

        //cout << "\nRef L0 ";      
	    for(int ii=0;ii<4;ii++)
	    {			    
            //cout << mb_ref_l0[ii] <<" , ";          
	        erroR |= abs(mb_ref_l0[ii] - rd_ref_l0[ii]);        
	    }
	    //cout << "\nRef L1 "; 
			for(int ii=0;ii<4;ii++)
	    {
		    
            //cout << mb_ref_l1[ii] <<" , ";          
	        erroR |= (abs(mb_ref_l1[ii] - rd_ref_l1[ii])); 
	    }
	    
		for(int ii=0;ii<16;ii++)
	    {
		    //if (!(ii%4)) cout << '\n';      
            //cout << mb_mv_l0[ii][0] <<" , "<< mb_mv_l0[ii][1]<<" , ";          
	        erroM |= ((abs(mb_mv_l0[ii][0] - rd_mv_l0[ii][0])) ||  (abs(mb_mv_l0[ii][1] - rd_mv_l0[ii][1])));        
	    }
			for(int ii=0;ii<16;ii++)
	    {
		    //if (!(ii%4)) cout << '\n';      
            //cout << mb_mv_l1[ii][0] <<" , "<< mb_mv_l1[ii][1]<<" , ";          
	        erroM |= (abs(mb_mv_l1[ii][0] - rd_mv_l1[ii][0])) ||  (abs(mb_mv_l1[ii][1] - rd_mv_l1[ii][1]));        
	    }
			
	    
	    
	    
	    if(erroR || erroM)
	    {
		     slice_type = -1;
		     if (erroR && erroM)
		     	cout << "\n\nERRO TOTAL ERRO \n" << " MB: " << counter << " Slice: " << icounter ;
		     if (erroR)
		     	cout << "\n\nERRO REF ERRO \n"<< " MB: " << counter << " Slice: " << icounter;
		     else cout << "\n\nERRO MOV ERRO \n"<< " MB: " << counter << " Slice: " << icounter;
	    }
	    if (counter > 98)
		{
			counter = 0;
			icounter++;
			if (icounter%3) slice_type = 1;
			else slice_type = 0;

			//slice_type = (slice_type + 1)%2;
		}
	    //cout << "\nReferencias" << counter++;
	  /*  for(int ii=0;ii<4;ii++)
	    {
		    cout << "[" << mb_ref_l0[ii]<<"]";        
	    }*/
	    
	       
	  //  cout << "\nentre slice_type"; cin >> trash >> slice_type;
	}
	/*times(&after);
	t1= time(NULL);
	local1=localtime(&t1);
	printf("antes [%d] depois[%d] diff [%d]", local->tm_sec, local1->tm_sec, local1->tm_sec - local->tm_sec);
	
	printf("User time: %ld seconds\n", after.tms_utime -
        before.tms_utime);
    printf("System time: %ld seconds\n", after.tms_stime -
        before.tms_stime);
        
        printf("Data do Sistema: %d\n",local->tm_mday);*/

}
