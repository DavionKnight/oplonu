#ifdef INCLUDE_TABLE_RW_FUNC

void rTab(int tid,int  item)
{
	int i=0,x;
	int val;
	switch(tid) {
		
	case 0 :                                                                                         
		printf ("read Bridge Port table\n");                                                               
		printf ("table record item %d\n",item);                                                                   
		x= item + 0x80000000;                                                             
        *((int *)( 0xbf020008 ))=x;                                                                         
		for ( i=0 ; i<=11; i++){                                                       
			val=*((int *)(  0xbf02000c +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}                                                                                           
			 printf("\n");break;                                                                                              
	case 1:                                                                                          
		printf ("read Bridge Address Resolution table\n");                                                 
		printf ("table record item %d\n",item);                                                                                                                                      
		x= item + 0x80010000;                                                             
		*((int *)( 0xbf020008 ))=x;                                                                         
		for ( i=0 ; i<=11; i++){                                                       
			val=*((int *)(  0xbf02000c +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}                                                                                           
			 printf("\n");break;                                                                                              
	case 2:                                                                                          
		printf ("read Bridge Vlan Pointer table\n");                                                       
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80020000;                                                             
		*((int *)( 0xbf020008 ))=x;                                                                         
		for ( i=0 ; i<= 11; i++){                                                      
			val=*((int *)(  0xbf02000c +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}                                                                                           
			 printf("\n");break;                                                                                              
	case 3:                                                                                          
		printf ("read Bridge Vlan Hash Table A\n");                                                        
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80030000;                                                             
		*((int *)( 0xbf020008 ))=x;                                                                         
		for ( i=0 ; i<= 11; i++){                                                      
			val=*((int *)(  0xbf02000c +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}                                                                                           
			 printf("\n");break;                                                                                              
	case 4:                                                                                          
		printf ("read Bridge Vlan Hash Table B\n");                                                        
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80040000;                                                             
		*((int *)( 0xbf020008 ))=x;                                                                         
		for ( i=0 ; i<= 11; i++){                                                      
			val=*((int *)(  0xbf02000c +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}                                                                                           
			 printf("\n");break;                                                                                              
	case 5:                                                                                          
		printf ("read Bridge Vlan Table \n");                                                              
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80050000;                                                             
		*((int *)( 0xbf020008 ))=x;                                                                         
		for ( i=0 ; i<= 11; i++){                                                      
			val=*((int *)(  0xbf02000c +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}                                                                                           
			 printf("\n");break;                                                                                              
	case 6:                                                                                          
		printf ("read Bridge CTAG Replacement Table \n");                                                  
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80060000;                                                             
		*((int *)( 0xbf020008 ))=x;                                                                         
		for ( i=0 ; i<= 11; i++){                                                      
			val=*((int *)(  0xbf02000c +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}                                                                                           
			 printf("\n");break;                                                                                              
	case 7:                                                                                          
		printf ("read Bridge VLAN Cross Connection Enable Table \n");                                      
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80070000;                                                             
		*((int *)( 0xbf020008 ))=x;                                                                         
		for ( i=0 ; i<= 11; i++){                                                      
			val=*((int *)(  0xbf02000c +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}                                                                                           
			 printf("\n");break;                                                                                              
	case 8:                                                                                          
		printf ("read Bridge MAC Filter Table \n");                                                        
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80080000;                                                             
		*((int *)( 0xbf020008 ))=x;                                                                         
		for ( i=0 ; i<= 11; i++){                                                      
			val=*((int *)(  0xbf02000c +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}                                                                                           
			 printf("\n");break;                                                                                              
	case 9:                                                                                          
		printf ("read Bridge Remark Table \n");                                                            
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80090000;                                                             
		*((int *)( 0xbf020008 ))=x;                                                                         
		for ( i=0 ; i<= 11; i++){                                                      
			val=*((int *)(  0xbf02000c +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}                                                                                           
			 printf("\n");break;                                                                                              
	case 10:                                                                                            
		printf ("read Bridge Multicast Group Table \n");                                                   
		printf ("table record %d\n",item);                                                                   
		x= item + 0x800a0000;                                                             
		*((int *)( 0xbf020008 ))=x;                                                                         
		for ( i=0 ; i<= 11; i++){                                                      
			val=*((int *)(  0xbf02000c +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}                                                                                           
			  printf("\n");break;                                                                                              
	case 11:                                                                                            
		printf ("read Bridge Multicast Entry Table \n");                                                   
		printf ("table record %d\n",item);                                                                   
		x= item + 0x800b0000;                                                             
		*((int *)( 0xbf020008 ))=x;                                                                         
		for ( i=0 ; i<= 11; i++){                                                      
			val=*((int *)(  0xbf02000c +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}                                                                                           
			  printf("\n");break;                                                                                              
	case 12:                                                                                            
		printf ("read Bridge Port Egress Control Table \n");                                               
		printf ("table record %d\n",item);                                                                   
		x= item + 0x800c0000;                                                             
		*((int *)( 0xbf020008 ))=x;                                                                         
		for ( i=0 ; i<= 11; i++){                                                      
			val=*((int *)(  0xbf02000c +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}                                                                                           
			  printf("\n");break;                                                                                              
	case 13:                                                                                            
		printf ("read Bridge MAC Address Limitation Table \n");                                            
		printf ("table record %d\n",item);                                                                   
		x= item + 0x800d0000;                                                             
		*((int *)( 0xbf020008 ))=x;                                                                         
		for ( i=0 ; i<= 11; i++){                                                      
			val=*((int *)(  0xbf02000c +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}                                                                                           
			  printf("\n");break;                                                                                              
	case 14:                                                                                            
		printf ("read Bridge MAC Address Limitation Counter Table \n");                                    
		printf ("table record %d\n",item);                                                                   
		x= item + 0x800e0000;                                                             
		*((int *)( 0xbf020008 ))=x;                                                                         
		for ( i=0 ; i<= 11; i++){                                                      
			val=*((int *)(  0xbf02000c +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}                                                                                           
			  printf("\n");break;                                                                                              
	case 16:                                                                                            
		printf ("read Classify Field Mask Table \n");                                                      
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80100000;                                                             
		*((int *)( 0xbf0180FC ))=x;                                                                         
		for ( i=0 ; i<= 18; i++){                                                      
			val=*((int *)(  0xbf018100 +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}                                                                                           
			  printf("\n");break;                                                                                              
	case 17:                                                                                            
		printf ("read Classify Rule hash Table \n");                                                       
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80110000;                                                             
		*((int *)( 0xbf0180FC ))=x;                                                                         
		for ( i=0 ; i<= 18; i++){                                                      
			val=*((int *)(  0xbf018100 +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}                                                                                           
			  printf("\n");break;                                                                                              
	case 18:                                                                                            
		printf ("read Classify Rule Table \n");                                                            
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80120000;                                                             
		*((int *)( 0xbf0180FC ))=x;                                                                         
		for ( i=0 ; i<= 18; i++){                                                      
			val=*((int *)(  0xbf018100 +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}                                                                                           
			  printf("\n");break;                                                                                              
	case 20:                                                                                            
		printf ("read Classify Flow ID mask Table \n");                                                    
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80140000;                                                             
		*((int *)( 0xbf0180FC ))=x;                                                                         
		for ( i=0 ; i<= 18; i++){                                                      
			val=*((int *)(  0xbf018100 +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}                                                                                           
			  printf("\n");break;                                                                                              
	case 21:                                                                                            
		printf ("read Classify Flow Hash Table \n");                                                       
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80150000;                                                             
		*((int *)( 0xbf0180FC ))=x;                                                                         
		for ( i=0 ; i<= 18; i++){                                                      
			val=*((int *)(  0xbf018100 +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}                                                                                           
			  printf("\n");break;                                                                                              
	case 22:                                                                                            
		printf ("read Classify Flow Table \n");                                                            
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80160000;                                                             
		*((int *)( 0xbf0180FC ))=x;                                                                         
		for ( i=0 ; i<= 18; i++){                                                      
			val=*((int *)(  0xbf018100 +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}                                                                                           
			  printf("\n");break;                                                                                              
	case 23:                                                                                            
		printf ("read Classify Flow Remark Table \n");                                                     
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80170000;                                                             
		*((int *)( 0xbf0180FC ))=x;                                                                         
		for ( i=0 ; i<= 18; i++){                                                      
			val=*((int *)(  0xbf018100 +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}                                                                                           
	
	case 24:                                                                                            
		printf ("Classify Hit Counter Table \n");                                                     
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80180000;                                                             
		*((int *)( 0xbf0180FC ))=x;                                                                         
		for ( i=0 ; i<= 18; i++){                                                      
			val=*((int *)(  0xbf018100 +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}          
	case 32:                                                                                            
		printf ("read Upstream Flow Shaper ID Table \n");                                                  
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80200000;                                                             
		*((int *)( 0xbf01412c ))=x;			                                                                
		val=*((int *)( 0xbf014130));                                                                  
		printf("%08x-", val);if(6==i) printf("\n");			                                                    
			  printf("\n");break;                                                                                              
	case 34:                                                                                            
		printf ("read Upstream Flow WRED ID Table \n");                                                    
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80220000;                                                             
		*((int *)( 0xbf01412c ))=x;			                                                                
		val=*((int *)( 0xbf014130));                                                                  
		printf("%08x-", val);if(6==i) printf("\n");			                                                    
			  printf("\n");break;                                                                                              
	case 36:                                                                                            
		printf ("read Upstream Flow ID table  R \n");                                                      
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80240000;                                                             
		*((int *)( 0xbf01412c ))=x;			                                                                
		val=*((int *)( 0xbf014130));                                                                  
		printf("%08x-", val);if(6==i) printf("\n");			                                                    
			  printf("\n");break;       
	case 44:                                                                                            
		printf ("read Upstream Flow Rx Packet Drop Counter Table\n");                                                      
		printf ("table record %d\n",item);                                                                   
		x= item + 0x802c0000;                                                             
		*((int *)( 0xbf01412c ))=x;			                                                                
		val=*((int *)( 0xbf014130));                                                                  
		printf("%08x-", val);if(6==i) printf("\n");			                                                    
			  printf("\n");break;       	
	case 51:                                                                                            
		printf ("read Upstream Flow Rx Packet Drop Counter Table\n");                                                      
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80330000;                                                             
		*((int *)( 0xbf01412c ))=x;			                                                                
		val=*((int *)( 0xbf014130));                                                                  
		printf("%08x-", val);if(6==i) printf("\n");			                                                    
			  printf("\n");break;      
	case 38:                                                                                            
		printf ("read Upstream Flow ID table W, but this is a write only table \n");						
			  printf("\n");break;                                                                                              
	case 49:                                                                                            
		printf ("read Dowstream Flow Shaper ID Table \n");                                                 
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80310000;                                                             
		*((int *)( 0xbf014e08 ))=x;                                                                         
		for ( i=0 ; i<= 9; i++){                                                       
			val=*((int *)(  0xbf018e0c +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}			                                                                                
		
	case 53:                                                                                            
		printf ("read Dowstream Flow Port Sequence table R \n");                                           
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80350000;                                                             
		*((int *)( 0xbf014e08 ))=x;                                                                         
		for ( i=0 ; i<= 9; i++){                                                       
			val=*((int *)(  0xbf018e0c +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}			                                                                                
			  printf("\n");break;                                                                                              
	case 55:                                                                                            
		printf ("read Dowstream Flow Port Weight and Priority table R \n");                                
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80370000;                                                             
		*((int *)( 0xbf014e08 ))=x;                                                                         
		for ( i=0 ; i<= 9; i++){                                                       
			val=*((int *)(  0xbf018e0c +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}			                                                                                
			  printf("\n");break;                                                                                              
	case 57:                                                                                            
		printf ("read Downstram Port Weight Sum Table R \n");                                              
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80390000;                                                             
		*((int *)( 0xbf014e08 ))=x;                                                                         
		for ( i=0 ; i<= 9; i++){                                                       
			val=*((int *)(  0xbf018e0c +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}			                                                                                
			  printf("\n");break;                                                                                              
	case 59:                                                                                            
		printf ("read Downstram flow Table W, but this is a write only table \n");						    
			  printf("\n");break;                                                                                              
	case 64:                                                                                            
		printf ("read SAR UPPT Table \n");                                                                 
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80400000;                                                             
		*((int *)( 0xbf013C04 ))=x;                                                                         
		for ( i=0 ; i<= 3; i++){                                                       
			val=*((int *)(  0xbf013C08 +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}			                                                                                
			  printf("\n");break;                                                                                              
	case 65:                                                                                            
		printf ("read SAR EPPT Table \n");                                                                 
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80410000;                                                             
		*((int *)( 0xbf013C04 ))=x;                                                                         
		for ( i=0 ; i<= 3; i++){                                                       
			val=*((int *)(  0xbf013C08 +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}			                                                                                
			  printf("\n");break;                                                                                              
	case 68:                                                                                            
		printf ("read Access port ctrl Table \n");                                                         
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80440000;                                                             
		*((int *)( 0xbf013C04 ))=x;                                                                         
		for ( i=0 ; i<= 0; i++){                                                       
			val=*((int *)(  0xbf013C08 +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}			                                                                                
			  printf("\n");break;        
	case 69:                                                                                            
		printf ("read SAR Access Port Rx Counter Group Table\n");                                                         
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80450000;                                                             
		*((int *)( 0xbf013C04 ))=x;                                                                         
		for ( i=0 ; i<= 0; i++){                                                       
			val=*((int *)(  0xbf013C08 +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}			                                                                                
			  printf("\n");break;     
	case 70:                                                                                            
		printf ("read SAR Access Port Rx Counter Group Table\n");                                                         
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80460000;                                                             
		*((int *)( 0xbf013C04 ))=x;                                                                         
		for ( i=0 ; i<= 0; i++){                                                       
			val=*((int *)(  0xbf013C08 +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}			                                                                                
			  printf("\n");break;     
	case 71:                                                                                            
		printf ("SAR Port VLAN Counter Pointer Table\n");                                                         
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80470000;                                                             
		*((int *)( 0xbf013C04 ))=x;                                                                         
		for ( i=0 ; i<= 0; i++){                                                       
			val=*((int *)(  0xbf013C08 +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}			                                                                                
			  printf("\n");break;               
	case 72:                                                                                            
		printf ("read SAR Port VLAN Counter Hash Table\n");                                                         
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80480000;                                                             
		*((int *)( 0xbf013C04 ))=x;                                                                         
		for ( i=0 ; i<= 0; i++){                                                       
			val=*((int *)(  0xbf013C08 +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}			                                                                                
			  printf("\n");break;    			  
	case 73:                                                                                            
		printf ("read SAR Port VLAN Counter Hash Table\n");                                                         
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80490000;                                                             
		*((int *)( 0xbf013C04 ))=x;                                                                         
		for ( i=0 ; i<= 0; i++){                                                       
			val=*((int *)(  0xbf013C08 +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}			                                                                                
			  printf("\n");break;       
	case 74:                                                                                            
		printf ("read SAR Port VLAN Counter Hash Table\n");                                                         
		printf ("table record %d\n",item);                                                                   
		x= item + 0x804a0000;                                                             
		*((int *)( 0xbf013C04 ))=x;                                                                         
		for ( i=0 ; i<= 0; i++){                                                       
			val=*((int *)(  0xbf013C08 +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}			                                                                                
			  printf("\n");break;    

case 75:                                                                                            
		printf ("SAR Port VLAN Counter Pointer Table\n");                                                         
		printf ("table record %d\n",item);                                                                   
		x= item + 0x804b0000;                                                             
		*((int *)( 0xbf013C04 ))=x;                                                                         
		for ( i=0 ; i<= 0; i++){                                                       
			val=*((int *)(  0xbf013C08 +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}			                                                                                
			  printf("\n");break;               
	case 76:                                                                                            
		printf ("read SAR Port VLAN Counter Hash Table\n");                                                         
		printf ("table record %d\n",item);                                                                   
		x= item + 0x804c0000;                                                             
		*((int *)( 0xbf013C04 ))=x;                                                                         
		for ( i=0 ; i<= 0; i++){                                                       
			val=*((int *)(  0xbf013C08 +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}			                                                                                
			  printf("\n");break;    			  
	case 77:                                                                                            
		printf ("read SAR Port VLAN Counter Hash Table\n");                                                         
		printf ("table record %d\n",item);                                                                   
		x= item + 0x804d0000;                                                             
		*((int *)( 0xbf013C04 ))=x;                                                                         
		for ( i=0 ; i<= 0; i++){                                                       
			val=*((int *)(  0xbf013C08 +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}			                                                                                
			  printf("\n");break;       
	case 78:                                                                                            
		printf ("read SAR Port VLAN Counter Hash Table\n");                                                         
		printf ("table record %d\n",item);                                                                   
		x= item + 0x804e0000;                                                             
		*((int *)( 0xbf013C04 ))=x;                                                                         
		for ( i=0 ; i<= 0; i++){                                                       
			val=*((int *)(  0xbf013C08 +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}			                                                                                
			  printf("\n");break;    
	case 80:                                                                                            
		printf ("read DMA0 RxBD table\n");                                                                 
		x= item + 0x80500000;                                                             
		*((int *)( 0xbf002b44 ))=x;                                                                         
		val=*((int *)( 0xbf002b40));                                                                  
		printf("0x%08x\t0x%08x", item, val);                                                  
			  printf("\n");break;                                                                                              
	case 81:                                                                                            
		printf ("read DMA0 TxBD table\n");                                                                 
		x= item + 0x80510000;                                                             
		*((int *)( 0xbf002b44 ))=x;                                                                         
		val=*((int *)( 0xbf002b40));                                                                  
		printf("0x%08x\t0x%08x", item, val);                                                  
			  printf("\n");break;                                                                                              
	case 96:                                                                                            
		printf ("read DMA1 RxBD table\n");                                                                 
		x= item + 0x80600000;                                                             
		*((int *)( 0xbf002bc4 ))=x;                                                                         
		val=*((int *)( 0xbf002bc0));                                                                  
		printf("0x%08x\t0x%08x", item, val);                                                  
			  printf("\n");break;                                                                                              
	case 97:                                                                                            
		printf ("read DMA1 TxBD table\n");                                                                 
		x= item + 0x80610000;                                                             
		*((int *)( 0xbf002bc4 ))=x;                                                                         
		val=*((int *)( 0xbf002bc0));                                                                  
		printf("0x%08x\t0x%08x", item, val);                                                  
		printf("\n");break;                                                                                              

	case 112:                                                                                            
		printf ("read SAR Port VLAN Counter Hash Table\n");                                                         
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80700000;                                                             
		*((int *)( 0xbf010100 ))=x;                                                                         
		for ( i=0 ; i<= 0; i++){                                                       
			val=*((int *)(  0xbf010104 +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}			                                                                                
			  printf("\n");break;    
	case 113:                                                                                            
		printf ("read SAR Port VLAN Counter Hash Table\n");                                                         
		printf ("table record %d\n",item);                                                                   
		x= item + 0x80710000;                                                             
		*((int *)( 0xbf010100 ))=x;                                                                         
		for ( i=0 ; i<= 0; i++){                                                       
			val=*((int *)(  0xbf010104 +i *4));                                                
			printf("%08x-", val);if(6==i) printf("\n");                                                            
		}			                                                                                
			  printf("\n");break;    
	case 114:                                                                                            
		printf ("read DMA0 RxBD table\n");                                                                 
		x= item + 0x80720000;                                                             
		*((int *)( 0xbf010100 ))=x;                                                                         
		val=*((int *)( 0xbf010104));                                                                  
		printf("0x%08x\t0x%08x", item, val);                                                  
			  printf("\n");break;                                                                                              
	case 115:                                                                                            
		printf ("read DMA0 TxBD table\n");                                                                 
		x= item + 0x80730000;                                                             
		*((int *)( 0xbf010100 ))=x;                                                                         
		val=*((int *)( 0xbf010104));                                                                  
		printf("0x%08x\t0x%08x", item, val);                                                  
			  printf("\n");break;                                                                                              
	case 116:                                                                                            
		printf ("read DMA1 RxBD table\n");                                                                 
		x= item + 0x80740000;                                                             
		*((int *)( 0xbf010100 ))=x;                                                                         
		val=*((int *)( 0xbf010104));                                                                  
		printf("0x%08x\t0x%08x", item, val);                                                  
			  printf("\n");break;                                                                                              
	case 117:                                                                                            
		printf ("read DMA1 TxBD table\n");                                                                 
		x= item + 0x80750000;                                                             
		*((int *)( 0xbf010100 ))=x;                                                                         
		val=*((int *)( 0xbf010104));                                                                  
		printf("0x%08x\t0x%08x", item, val);                                                  
		printf("\n");break;                                                                                              
		
	default:
		printf("Not realized\n");	
		break;
	}
}
void wTabVal(int tid,int  numth,int  val)
{
	
	switch (tid){

	case 0:
		*((int *)(  0xbf02000c +numth *4))=val;
		printf("0x%08x\n", val);
        break; 
	case 1 :
		*((int *)(  0xbf02000c +numth *4))=val;
		printf("0x%08x\n", val);
        break; 
	case 2 :
		*((int *)(  0xbf02000c +numth *4))=val;
		printf("0x%08x\n", val);
        break; 
	case 3 :
		*((int *)(  0xbf02000c +numth *4))=val;
		printf("0x%08x\n", val);
        break; 
	case 4 :
		*((int *)(  0xbf02000c +numth *4))=val;
		printf("0x%08x\n", val);
        break; 
	case 5 :
		*((int *)(  0xbf02000c +numth *4))=val;
		printf("0x%08x\n", val);
        break; 
	case 6 :
		*((int *)(  0xbf02000c +numth *4))=val;
		printf("0x%08x\n", val);
        break; 
	case 7 :
		*((int *)(  0xbf02000c +numth *4))=val;
		printf("0x%08x\n", val);
        break; 
	case 8 :
		*((int *)(  0xbf02000c +numth *4))=val;
		printf("write addr 0x%08x val 0x%08x\n",0xbf02000c +numth *4, val);
        break; 
	case 9 :
		*((int *)(  0xbf02000c +numth *4))=val;
		printf("0x%08x\n", val);
        break; 
	case 10 :
		*((int *)(  0xbf02000c +numth *4))=val;
		printf("0x%08x\n", val);
        break; 
	case 11 :
		*((int *)(  0xbf02000c +numth *4))=val;
		printf("0x%08x\n", val);
        break; 
	case 12 :
		*((int *)(  0xbf02000c +numth *4))=val;
		printf("0x%08x\n", val);
        break; 
	case 16:
		*((int *)(  0xbf018100 +numth *4))=val;
		printf("0x%08x\n", val);
        break; 
	case 17:
		*((int *)(  0xbf018100 +numth *4))=val;
		printf("0x%08x\n", val);
        break; 
	case 18:
		*((int *)(  0xbf018100 +numth *4))=val;
		printf("0x%08x\n", val);
        break; 
	case 20:
		*((int *)(  0xbf018100 +numth *4))=val;
		printf("0x%08x\n", val);
        break; 
	case 21:
		*((int *)(  0xbf018100 +numth *4))=val;
		printf("0x%08x\n", val);
        break; 
	case 22:
		*((int *)(  0xbf018100 +numth *4))=val;
		printf("0x%08x\n", val);
        break; 
	case 23:
		*((int *)(  0xbf018100 +numth *4))=val;
		printf("0x%08x\n", val);
        break; 
		
	case 32:
		printf ("this is a single*((int *)( table, pls call wrtable directly");
        break; 
	case 34:
		printf ("this is a single*((int *)( table, pls call wrtable directly");
        break; 
	case 36:
		printf ("this is a read only table, NOT wrtable");
        break; 
	case 38:
		printf ("this is a single*((int *)( table, pls call wrtable directly");
        break; 
	case 49:
	case 44:
		printf ("this is a single*((int *)( table, pls call wrtable directly");
        break; 
	case 51:
		printf ("this is a single*((int *)( table, pls call wrtable directly");
        break; 
	case 53:
		printf ("this is a read only table, NOT wrtable");
        break; 
	case 55:
		printf ("this is a read only table, NOT wrtable");
        break; 
	case 57:
		*((int *)(  0xbf014e0c +numth *4))=val;
		printf("0x%08x\n", val);
        break; 
	case 64:
	case 65:
	case 68:
	case 71:
	case 72:
	case 73:		
	case 74:		
		*((int *)(  0xbf013c08 +numth *4))=val;
		printf("0x%08x\n", val);
        break; 

	case 80:
		printf ("this is a single*((int *)( table, pls call wrtable directly");
        break; 
	case 81:
		printf ("this is a single*((int *)( table, pls call wrtable directly");
        break; 
	case 96:
		printf ("this is a single*((int *)( table, pls call wrtable directly");
        break; 
	case 97:
		printf ("this is a single*((int *)( table, pls call wrtable directly");
        break; 
	default:
		printf ("select wrong table ID");
        break; 
	}

}
	
void wTabRec(int tid,int  item,int  val)
{

	int x;
	switch (tid){
	case 0 :
		printf ("write Bridge Port table");
		printf ("table record item %d\n",item);
		x= item + 0xc0000000;	
		*((int *)( 0xbf020008 ))=x;
		break;   
	case 1  :
		printf ("write Bridge Address Resolution table");
		printf ("table record item %d\n",item);
		x= item + 0xc0010000;	
		*((int *)( 0xbf020008 ))=x;
		break; 
	case 2  :
		printf ("write Bridge Vlan Pointer table");
		printf ("table record item %d\n",item);
		x= item + 0xc0020000;
		*((int *)( 0xbf020008 ))=x;
		break; 
	case 3  :
		printf ("write Bridge Vlan Hash Table A");
		printf ("table record item %d\n",item);
		x= item + 0xc0030000;
		*((int *)( 0xbf020008 ))=x;
		break; 
	case 4  :
		printf ("write Bridge Vlan Hash Table B");
		printf ("table record item %d\n",item);
		x= item + 0xc0040000;
		*((int *)( 0xbf020008 ))=x;
		break; 
	case 5  :
		printf ("write Bridge Vlan Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc0050000;
		*((int *)( 0xbf020008 ))=x;
		break; 
	case 6  :
		printf ("write Bridge CTAG Replacement Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc0060000;
		*((int *)( 0xbf020008 ))=x;
		break; 
	case 7  :
		printf ("write Bridge VLAN Cross Connection Enable Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc0070000;
		*((int *)( 0xbf020008 ))=x;
		break; 
	case 8  :
		printf ("write Bridge MAC Filter Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc0080000;
		printf("write addr 0x%x val 0x%x\n",0xbf020008,x);
		*((int *)( 0xbf020008 ))=x;
		
		break; 
	case 9  :
		printf ("write Bridge Remark Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc0090000;
		*((int *)( 0xbf020008 ))=x;
		break; 
	case 10 :
		printf ("write Bridge Multicast Group Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc00a0000;
		*((int *)( 0xbf020008 ))=x;
		break; 
	case 11 :
		printf ("write Bridge Multicast Entry Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc00b0000;
		*((int *)( 0xbf020008 ))=x;
		break; 
	case 12 :
		printf ("write Bridge Port Egress Control Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc00c0000;
		*((int *)( 0xbf020008 ))=x;
		break; 
	case 16 :
		printf ("write Classify Field Mask Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc0100000;	
		*((int *)( 0xbf0180FC ))=x;
		break; 
	case 17 :
		printf ("write Classify Rule hash Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc0110000;
		*((int *)( 0xbf0180FC ))=x;
		break; 
	case 18 :
		printf ("write Classify Rule Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc0120000;
		*((int *)( 0xbf0180FC ))=x;
		break; 
	case 20 :
		printf ("write Classify Flow ID mask Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc0140000;	
		*((int *)( 0xbf0180FC ))=x;
		break; 
	case 21 :
		printf ("write Classify Flow Hash Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc0150000;
		*((int *)( 0xbf0180FC ))=x;
		break; 
	case 22 :
		printf ("write Classify Flow Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc0160000;
		*((int *)( 0xbf0180FC ))=x;
		break; 
	case 23 :
		printf ("write Classify Flow Remark Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc0170000;
		*((int *)( 0xbf0180FC ))=x;
		break; 
	case 24 :
		printf ("Classify Hit Counter Table");
		printf ("table record item %d\n",item);
		x= item + 0xc0180000;
		*((int *)( 0xbf0180FC ))=x;
		break; 

	case 32 :
		printf ("write Upstream Flow Shaper ID Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc0200000;
		*((int *)( 0xbf014130))=val;
		*((int *)( 0xbf01412c ))=x;
		printf("0x%08x\n", val);     	
		break; 
	case 34 :
		printf ("write Upstream Flow WRED ID Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc0220000;
		*((int *)( 0xbf014130))=val;
		*((int *)( 0xbf01412c ))=x;
		printf("0x%08x\n", val);     	
		break; 
	case 36 :
		printf ("this is a read only table, NOT writable");     	
		break; 
	case 38 :
		printf ("write Upstream Flow Table W ");
		printf ("table record item %d\n",item);
		x= item + 0xc0260000;;	
		*((int *)( 0xbf014130))=val;
		*((int *)( 0xbf01412c ))=x;
		printf("0x%08x\n", val);
		break; 
	case 44 :
		printf ("write Upstream Flow Table W ");
		printf ("table record item %d\n",item);
		x= item + 0xc02c0000;;	
		*((int *)( 0xbf014130))=val;
		*((int *)( 0xbf01412c ))=x;
		printf("0x%08x\n", val);
		break; 
		
	case 49 :
		printf ("read Dowstream Flow Shaper ID Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc0310000; 
		*((int *)( 0xbf018e0c))=val;
		printf("0x%08x\n", val);
		*((int *)( 0xbf014e08 ))=x;     	
		break;
	
		
	case 51 :
		printf ("read Dowstream Flow WRED ID Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc0330000;
		*((int *)( 0xbf018e0c))=val;
		printf("0x%08x\n", val);
		*((int *)( 0xbf014e08 ))=x; 
		break; 
	case 53 :
		printf ("this is a read only table,NOT writable ");     	
		break;  
	case 55 :
		printf ("this is a read only table,NOT writable ");     	
		break; 
	case 57 :
		printf ("write Downstram Port Weight Sum Table w ");
		printf ("table record item %d\n",item);
		x= item + 0xc0390000;
		*((int *)( 0xbf014e08 ))=x;      	
		break; 
	case 59 :
		printf ("read Downstram flow Table W, but this is a write only table ");
		break;
	case 64 :
		printf ("write SAR UPPT Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc0400000;
		*((int *)( 0xbf013C04 ))=x;
		break; 
	case 65 :
		printf ("write SAR EPPT Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc0410000;	
		*((int *)( 0xbf013C04 ))=x;     	
		break; 
	case 68 :
		printf ("write IPMUX_TAB_SAR_CNT_APCTRL Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc0440000;	
		*((int *)( 0xbf013C04 ))=x;     	
		break; 
	case 71 :
		printf ("write SAR Port VLAN Counter Pointer Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc0470000;	
		*((int *)( 0xbf013C04 ))=x;     	
		break; 
	case 72 :
		printf ("write ISAR Port VLAN Counter Hash Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc0480000;	
		*((int *)( 0xbf013C04 ))=x;     	
		break; 
	case 73 :
		printf ("write ISAR Port VLAN Counter Hash Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc0490000;	
		*((int *)( 0xbf013C04 ))=x;     	
		break; 
	case 74 :
		printf ("write ISAR Port VLAN Counter Hash Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc04a0000;	
		*((int *)( 0xbf013C04 ))=x;     	
		break; 		
	case 75 :
		printf ("write SAR Port VLAN Counter Pointer Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc04b0000;	
		*((int *)( 0xbf013C04 ))=x;     	
		break; 
	case 76 :
		printf ("write ISAR Port VLAN Counter Hash Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc04c0000;	
		*((int *)( 0xbf013C04 ))=x;     	
		break; 
	case 77 :
		printf ("write ISAR Port VLAN Counter Hash Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc04d0000;	
		*((int *)( 0xbf013C04 ))=x;     	
		break; 
	case 78 :
		printf ("write ISAR Port VLAN Counter Hash Table ");
		printf ("table record item %d\n",item);
		x= item + 0xc04e0000;	
		*((int *)( 0xbf013C04 ))=x;     	
		break; 				
	case 80 :
		printf ("write DMA0 RxBD table");
		x= item + 0xc0500000;     	
		*((int *)( 0xbf002b40))=val;	
		*((int *)( 0xbf002b44 ))=x; 
		break; 
	case 81 :
		printf ("write DMA0 TxBD table");
		x= item + 0xc0510000;
		*((int *)( 0xbf002b40))=val;
		*((int *)( 0xbf002b44 ))=x;
		break; 
	case 96 :
		printf ("write DMA1 RxBD table");
		x= item + 0xc0600000;
		*((int *)( 0xbf002bc0))=val;
		*((int *)( 0xbf002bc4 ))=x;     	
		break; 
	case 97:
		printf ("write DMA1 TxBD table");
		x= item + 0xc0610000;
		*((int *)( 0xbf002bc0))=val;
		*((int *)( 0xbf002bc4 ))=x;     	
		break; 
	default:
		printf("Not realized\n");
		break;
	}       
	

}


void wtab(int tid, int offset,int val,int val1)
{
	wTabVal(tid,0,val);
	if (val1 !=0xAAAA3333)	wTabVal(tid,1,val1);
	wTabRec(tid,offset,val);
	printf("\n");
}

#endif