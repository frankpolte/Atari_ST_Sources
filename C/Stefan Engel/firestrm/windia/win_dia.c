/***********************************************************************/
/* My own dialog in windows rutines                                    */
/*                                                                     */
/* X Open dialogs                                                      */
/* X Closing dialogs                                                   */
/* X Moving of dialogs                                                 */
/* X Updating of dialogs                                               */
/* X Handling of menu bar (without short keys)                         */
/* x Handling of menu bar (with short keys)                            */
/*   Iconifying of window                                              */  
/* X Uniconify of Dialogs                                              */
/* X Handling of Buttons                                               */
/* X Handling of Default Button (Return)                               */
/* X Handling of Radio Buttons                                         */
/* . Handling of Text-input into a TED-OBJECT                          */
/* . Handling of Popups (No working slide)                             */
/* . Beeing able to hide dialog windows (closeing but not deleteing)   */
/* . Beeing able to unhide dialog windows                              */
/* X Automatic Topping of the Window at the Bottom                     */
/* X Automatic Bottoming of the Topped Window                          */
/* X NonModal Dialogs                                                  */
/***********************************************************************/
/***********************************************************************/
/* Include filer till win_dia.h                                        */
/***********************************************************************/
#include <aes.h>
#include <vdi.h>
#include <stdio.h>
#include <tos.h>
#include <string.h>
#include "windia.h"
#include "win_dia.h"
#include "win_dia2.h"
/***********************************************************************/
/* Loads the resource-file                                             */
/***********************************************************************/
int load_resource(const char *filename)
{
  return rsrc_load(filename);
}

/***********************************************************************/
/* Initierar grafiken                                                  */
/***********************************************************************/
int init_graph()
{
  wind_get(DESK,WF_FULLXYWH,&screenx,&screeny,&screenw,&screenh);
  graf_mouse(ARROW,NULL);
  graf_id=graf_handle(&dummy, &dummy, &dummy, &dummy);

  work_in[0]=graf_id;
  work_in[1]=SOLID;
  work_in[2]=BLACK;
  work_in[3]=SQUARE;
  work_in[4]=BLACK;
  work_in[5]=NOLL;
  work_in[6]=BLACK;
  work_in[7]=SOLID;
  work_in[8]=SOLID;
  work_in[9]=BLACK;
  work_in[10]=2;
  v_opnvwk(work_in,&graf_id,work_out);
  
  if(_AESglobal[0]<0x340)
  {
    if(appl_find("?AGI")>0)
    {
      appl_getinfo(AES_OBJECT,&xy[0],&xy[1],&xy[2],&xy[3]);
      exist_3d=xy[0];
    }
    else
      exist_3d=FALSE;
  }
  else
    exist_3d=TRUE;

  if(vq_gdos()!=GDOS_NONE)
    gdos=TRUE;
  else
    gdos=FALSE;

  if(gdos)
    num_fonts=vst_load_fonts(graf_id,NOLL);
  else
    num_fonts=0;
  vst_alignment(graf_id,F_LEFT,F_ASCENT,&dummy,&dummy);
  vqt_extent(graf_id," ",xy);
  font_height=xy[5];
  font_width=xy[4];
  vst_effects(graf_id,NOLL);
  rsrc_init();

  key_table=Keytbl((char*)-1,(char*)-1,(char*)-1);

  return(TRUE);
}
/***********************************************************************/
/***********************************************************************/
void deinit_graph()
{
  if(gdos)
    vst_unload_fonts(graf_id,NOLL);
  v_clsvwk(graf_id);
  graf_mouse(ARROW,NULL);
}
/***********************************************************************/
/* open_menu: sl�nger upp menyn p� sk�rmen och initierar short-keys    */
/***********************************************************************/
int open_menu(OBJECT *tree,int type)
{
  int  next=ROOT, last=ROOT, strcount;
  int  end,start,dummy,number=NOLL;
  char temp[40];
  
  if(menutree==NOLL)
  {
    menutree=tree;
    if(tree[next].ob_head!=FAIL)
      next=tree[next].ob_head;
    else
      next=tree[next].ob_next;
    do
    {
      if(tree[next].ob_type==G_STRING)
      {
        start=end=FAIL;
        for(strcount=0; tree[next].string[strcount]!='\0'; strcount++);
        for(strcount--;tree[next].string[strcount]==' ';strcount--); 
        end=strcount;
        for(;tree[next].string[strcount]!=' ';strcount--); 
        if((tree[next].string[strcount-1]==' ')&&(strcount>2))
        {
          start=strcount+1;
          for(dummy=start;dummy<=end;dummy++)
            temp[dummy-start]=tree[next].string[dummy];
          temp[dummy-start]='\0';
          end=end-start;
          start=0;
          shortkeys[number]=NOLL;
          shortkorr[number]=next;
          dummy=FALSE;
          do
          {
            if(temp[start]==CH_SHIFT)
              shortkeys[number]|=NU_SHIFT;
            else if(temp[start]==CH_CTRL)
              shortkeys[number]|=NU_CTRL;
            else if(temp[start]==CH_ALT)
              shortkeys[number]|=NU_ALT;
            else
              dummy=TRUE;
            if(!dummy)
              start++;
          }while(!dummy);
          if(start<end)
          {
            if(!strcmp(temp+start,CH_HELP))
              shortkeys[number]|=NU_SCAN|SC_HELP;
            else if(!strcmp(temp+start,CH_UNDO))
              shortkeys[number]|=NU_SCAN|SC_UNDO;
            else if(!strcmp(temp+start,CH_INSERT))
              shortkeys[number]|=NU_SCAN|SC_INSERT;
            else if(!strcmp(temp+start,CH_CLR))
              shortkeys[number]|=NU_SCAN|SC_CLR;
            else if(!strcmp(temp+start,CH_ESC))
              shortkeys[number]|=NU_ESC;
            else if(!strcmp(temp+start,CH_BACKSPC))
              shortkeys[number]|=NU_BACKSPC;
            else if(!strcmp(temp+start,CH_DELETE))
              shortkeys[number]|=NU_DELETE;
            else if(!strcmp(temp+start,CH_TAB))
              shortkeys[number]|=NU_TAB;
            else if(temp[start]=='F')
             {
               start++;
             }
          }
          else
            shortkeys[number]|=toupper(temp[start]);
          temp[0]=toupper(temp[start]);
          temp[1]=0;
          number++;
        } 
      }
      last=next;
  
      if(tree[next].ob_head!=FAIL)
        next=tree[next].ob_head;
      else 
      {
        next=tree[next].ob_next;
        while(next<last)
        {
          last=next;
          if(last==FAIL)
            next=ROOT;
          else
            next=tree[next].ob_next;
        }
      }
    }while(next!=ROOT);
    shortkorr[number]=NOLL;
  }
  if(!menu_bar(tree, type))
    return(FALSE);
  else
    return(TRUE);
}

/***********************************************************************/
/* open_dialog: Rutin f�r att sl�nga in Dialogboxen i ett f�nster      */
/*              Eller text/lista i ett f�nster                         */
/***********************************************************************/
int open_dialog(WIN_DIALOG *info,short win_xy[XYWH])
{
  short x,y,w,h;
  short xy[XYWH];
  info->attr&=~SMALLER;
  if(info->status==WINDOW_CLOSED)
  {
    info->ident=wind_create(info->attr, screenx, screeny, screenw, screenh);
    if(info->attr&NAME)
      wind_title(info->ident,info->w_name);
    if(info->attr&INFO)
      wind_info(info->ident,info->w_info);
    if(info->type==TYPE_DIALOG)
    {
      if(info->dialog!=NOLL)
      {
        if(win_xy==NULL)
        {
          b_place(info->dialog,ROOT,xy);
          wind_calc(WC_BORDER, info->attr, xy[X], xy[Y], xy[W], xy[H], &x, &y, &w, &h);
        }
        wind_open(info->ident, x, y, w, h);
        wind_get(info->ident,WF_WORKXYWH,&(info->xy[X]),&(info->xy[Y]),&(info->xy[W]),&(info->xy[H]));
        dias.diainfo[dias.number].text_object=search_for_text(info->dialog,ROOT);
        dias.diainfo[dias.number].text_pos=1;
        info->status=WINDOW_OPENED;
      }
      else
      {
        wind_delete(info->ident);
        info->ident=FAIL;
        info->status=WINDOW_CLOSED;
        return(FALSE);
      }
    }
#ifdef WINTEXT
    else if((info->type==TYPE_TEXT)||(info->type==TYPE_LIST))
    {
      if((win_xy==NULL)||(info->text==NULL))
      {
        wind_delete(info->ident);
        info->ident=FAIL;
        info->status=WINDOW_CLOSED;
        return(FALSE);
      }
      if(info->type==TYPE_TEXT)
      {
        if(info->text->textstart==NOLL)
        {
            wind_delete(info->ident);
            info->ident=FAIL;
            info->status=WINDOW_CLOSED;
            return(FALSE);
        }
        if(!convert_text(info,TRUE))
        {
          wind_delete(info->ident);
          info->ident=FAIL;
          info->status=WINDOW_CLOSED;
          return(FALSE);
        }
      }
      if((info->type==TYPE_LIST)&&((info->text->createline==NOLL)||(info->text->createnext==NOLL)))
      {
        wind_delete(info->ident);
        info->ident=FAIL;
        info->status=WINDOW_CLOSED;
        return(FALSE);
      }
      info->text->graf_id=graf_handle(&dummy, &dummy, &dummy, &dummy);
      work_in[0]=graf_id;
      work_in[1]=SOLID;
      work_in[2]=BLACK;
      work_in[3]=SQUARE;
      work_in[4]=BLACK;
      work_in[5]=NOLL;
      work_in[6]=BLACK;
      work_in[7]=SOLID;
      work_in[8]=SOLID;
      work_in[9]=BLACK;
      work_in[10]=2;
      v_opnvwk(work_in,&info->text->graf_id,work_out);
      vst_alignment(info->text->graf_id,F_LEFT,F_ASCENT,&dummy,&dummy);
      if(gdos)
        vst_load_fonts(info->text->graf_id,NOLL);
      vst_effects(info->text->graf_id,NOLL);
      vst_font(info->text->graf_id,info->text->font_id);
      vst_point(info->text->graf_id,info->text->font_size,&dummy,&dummy,&info->text->char_w,&info->text->char_h);
      info->status=WINDOW_REDRAW;
      wind_open(info->ident,win_xy[X], win_xy[Y], win_xy[W], win_xy[H]);
      wind_get(info->ident,WF_WORKXYWH,&(info->xy[X]),&(info->xy[Y]),&(info->xy[W]),&(info->xy[H]));
      xy[H]=info->xy[H];
      if(info->text->dialog!=NOLL)
      {
		((OBJECT*)info->text->dialog)[ROOT].ob_width=screenw;
        ((OBJECT*)info->text->dialog)[ROOT].ob_x=info->xy[X];
        ((OBJECT*)info->text->dialog)[ROOT].ob_y=info->xy[Y];
        xy[H]-=((OBJECT*)info->text->dialog)[ROOT].ob_height;
      }
      if(info->type==TYPE_TEXT)
      {
        set_text_sliders(info);
        if(xy[H]/info->text->char_h>info->text->num_of_rows)
          wind_set(info->ident,WF_VSLSIZE,1000,0,0,0);
        else
          wind_set(info->ident,WF_VSLSIZE,(SLIDESIZE*xy[H]/info->text->char_h)/info->text->num_of_rows,0,0,0);
      }
      else
      {
        wind_set(info->ident,WF_VSLSIZE,SLIDESIZE/info->text->num_of_rows,0,0,0);
        set_mark(info,info->text->select,FALSE);
      }
      set_text_sliders(info);
    }
#endif
    else
    {
      if(win_xy==NOLL)
      {
        wind_delete(info->ident);
        info->ident=FAIL;
        info->status=WINDOW_CLOSED;
        return(FALSE);
      }
      dias.diainfo[dias.number].text_object=FAIL;
      wind_open(info->ident, win_xy[X], win_xy[Y], win_xy[W], win_xy[H]);
      wind_get(info->ident,WF_WORKXYWH,&(info->xy[X]),&(info->xy[Y]),&(info->xy[W]),&(info->xy[H]));
      info->status=WINDOW_OPENED;
    }
    dias.window[dias.number]=info;
    dias.number++;
    info->order=dias.number;
  }
  else if((info->status==WINDOW_HIDDEN)||(info->status==WINDOW_IHIDDEN))
  {
    unhide_dialog(info);
  }
  else
  {
    int counter=0,found=FALSE;
    for(counter=0 ; counter<dias.number ; counter++)
      if((dias.window[counter]->status==WINDOW_NMOPENED)||(dias.window[counter]->status==WINDOW_NMICONIZED)||(dias.window[counter]->status==WINDOW_NMREDRAW))
        found=TRUE;
    if(info->attr&NAME)
      wind_title(info->ident,info->w_name);
    if(info->attr&INFO)
      wind_info(info->ident,info->w_info);
    wind_get(DESK,WF_TOP,&xy[Y4],&xy[X1],&xy[X1],&xy[X1]);
    if((!found)||(info->order==dias.number))
      wind_set(info->ident,WF_TOP,info->ident,NOLL,NOLL,NOLL);
#ifdef WINTEXT
    if(info->type==TYPE_TEXT)
    {
      convert_text(info,FALSE);
      if(!convert_text(info,TRUE))
      {
        close_dialog(info);
        return(FALSE);
      }
      set_text_sliders(info);
      if(xy[H]/info->text->char_h>info->text->num_of_rows)
        wind_set(info->ident,WF_VSLSIZE,1000,0,0,0);
      else
        wind_set(info->ident,WF_VSLSIZE,(SLIDESIZE*xy[H]/info->text->char_h)/info->text->num_of_rows,0,0,0);
    }
    if(info->type==TYPE_LIST)
    {
      wind_set(info->ident,WF_VSLSIZE,SLIDESIZE/info->text->num_of_rows,0,0,0);
      set_text_sliders(info);
      set_mark(info,info->text->select,FALSE);
    }
#endif
    redraw_window(info);
  }
  return(TRUE);
}

/***********************************************************************/
/* open_nonmodal: Rutin f�r att sl�nga in Dialogboxen i ett f�nster    */
/*                Eller text/lista i ett f�nster, detta �ppnande ser   */
/*                ser till at detta f�nster blir det enda med kontroll */
/***********************************************************************/
int open_nonmodal(WIN_DIALOG *info,short win_xy[XYWH])
{
  short x,y,w,h;
  int next,last,first_title,first_string;
  
  if(info->status==WINDOW_CLOSED)
  {
    info->ident=wind_create(info->attr, screenx, screeny, screenw, screenh);
    if(info->attr&NAME)
      wind_title(info->ident,info->w_name);
    if(info->attr&INFO)
      wind_info(info->ident,info->w_info);
    if(info->type==TYPE_DIALOG)
    {
      if(info->dialog!=NOLL)
      {
        if(win_xy==NULL)
        {
          b_place(info->dialog,ROOT,xy);
          wind_calc(WC_BORDER, info->attr, xy[X], xy[Y], xy[W], xy[H], &x, &y, &w, &h);
        }
        wind_open(info->ident, x, y, w, h);
        wind_get(info->ident,WF_WORKXYWH,&(info->xy[X]),&(info->xy[Y]),&(info->xy[W]),&(info->xy[H]));
        dias.diainfo[dias.number].text_object=search_for_text(info->dialog,ROOT);
        dias.diainfo[dias.number].text_pos=1;
        if(dias.diainfo[dias.number].text_object!=ROOT)
          objc_edit(info->dialog,dias.diainfo[dias.number].text_object,0,&dias.diainfo[dias.number].text_pos,ED_INIT);
        info->status=WINDOW_NMOPENED;
      }
      else
      {
        wind_delete(info->ident);
        info->ident=FAIL;
        info->status=WINDOW_CLOSED;
        return(FALSE);
      }
    }
#ifdef WINTEXT
    else if((info->type==TYPE_TEXT)||(info->type==TYPE_LIST))
    {
      if((win_xy==NULL)||(info->text==NULL))
      {
        wind_delete(info->ident);
        info->ident=FAIL;
        info->status=WINDOW_CLOSED;
        return(FALSE);
      }
      if(info->type==TYPE_TEXT)
      {
        if(info->text->textstart==NOLL)
        {
            wind_delete(info->ident);
            info->ident=FAIL;
            info->status=WINDOW_CLOSED;
            return(FALSE);
        }
        if(!convert_text(info,TRUE))
        {
          wind_delete(info->ident);
          info->ident=FAIL;
          info->status=WINDOW_CLOSED;
          return(FALSE);
        }
      }
      if((info->type==TYPE_LIST)&&((info->text->createline==NOLL)||(info->text->createnext==NOLL)))
      {
        wind_delete(info->ident);
        info->ident=FAIL;
        info->status=WINDOW_CLOSED;
        return(FALSE);
      }
      info->text->graf_id=graf_handle(&dummy, &dummy, &dummy, &dummy);
      work_in[0]=graf_id;
      work_in[1]=SOLID;
      work_in[2]=BLACK;
      work_in[3]=SQUARE;
      work_in[4]=BLACK;
      work_in[5]=NOLL;
      work_in[6]=BLACK;
      work_in[7]=SOLID;
      work_in[8]=SOLID;
      work_in[9]=BLACK;
      work_in[10]=2;
      v_opnvwk(work_in,&info->text->graf_id,work_out);
      vst_alignment(info->text->graf_id,F_LEFT,F_ASCENT,&dummy,&dummy);
      if(gdos)
        vst_load_fonts(info->text->graf_id,NOLL);
      vst_effects(info->text->graf_id,NOLL);
      vst_point(info->text->graf_id,-1,&dummy,&dummy,&info->text->char_w,&info->text->char_h);
      info->status=WINDOW_NMREDRAW;
      wind_open(info->ident,win_xy[X], win_xy[Y], win_xy[W], win_xy[H]);
      wind_get(info->ident,WF_WORKXYWH,&(info->xy[X]),&(info->xy[Y]),&(info->xy[W]),&(info->xy[H]));
      set_mark(info,info->text->select,FALSE);
      xy[H]=info->xy[H];
      if(info->text->dialog!=NOLL)
      {
		((OBJECT*)info->text->dialog)[ROOT].ob_width=screenw;
        ((OBJECT*)info->text->dialog)[ROOT].ob_x=info->xy[X];
        ((OBJECT*)info->text->dialog)[ROOT].ob_y=info->xy[Y];
        xy[H]-=((OBJECT*)info->text->dialog)[ROOT].ob_height;
      }
      if(info->type==TYPE_TEXT)
      {
        set_text_sliders(info);
        if(xy[H]/info->text->char_h>info->text->num_of_rows)
          wind_set(info->ident,WF_VSLSIZE,1000,0,0,0);
        else
          wind_set(info->ident,WF_VSLSIZE,(SLIDESIZE*xy[H]/info->text->char_h)/info->text->num_of_rows,0,0,0);
      }
      else
        wind_set(info->ident,WF_VSLSIZE,SLIDESIZE/info->text->num_of_rows,0,0,0);
        set_text_sliders(info);
    }
#endif
    else
    {
      if(win_xy==NOLL)
      {
        wind_delete(info->ident);
        info->ident=FAIL;
        info->status=WINDOW_CLOSED;
        return(FALSE);
      }
      dias.diainfo[dias.number].text_object=FAIL;
      wind_open(info->ident, win_xy[X], win_xy[Y], win_xy[W], win_xy[H]);
      wind_get(info->ident,WF_WORKXYWH,&(info->xy[X]),&(info->xy[Y]),&(info->xy[W]),&(info->xy[H]));
      info->status=WINDOW_NMOPENED;
    }
    dias.window[dias.number]=info;
    dias.number++;
    info->order=dias.number;
    dias.nonmodal++;

    next=last=ROOT;
    first_title=TRUE;
    if(menutree[next].ob_head!=FAIL)
      next=menutree[next].ob_head;
    else
      next=menutree[next].ob_next;
    first_string=TRUE;
    do
    {
      if(menutree[next].ob_type==G_TITLE)
      {
        if(!first_title)
        {
          if (menutree[next].ob_state&DISABLED)
            menutree[next].ob_state|=SPECIAL;
          menutree[next].ob_state|=DISABLED;
        }
        first_title=FALSE;
      }
      else if(menutree[next].ob_type==G_STRING)
      {
        if(first_string)
        {
          if (menutree[next].ob_state&DISABLED)
            menutree[next].ob_state|=SPECIAL;
          menutree[next].ob_state|=DISABLED;
        }
        first_string=FALSE;
      }
      last=next;
  
      if(menutree[next].ob_head!=FAIL)
        next=menutree[next].ob_head;
      else 
      {
        next=menutree[next].ob_next;
        while(next<last)
        {
          last=next;
          if(last==FAIL)
            next=ROOT;
          else
            next=menutree[next].ob_next;
        }
      }
    }while(next!=ROOT);
    menu_bar(menutree, 1);
  }
  else if((info->status==WINDOW_HIDDEN)||(info->status==WINDOW_IHIDDEN))
  {
    unhide_dialog(info);
  }
  else
  {
    int counter=0,found=FALSE;
    for(counter=0 ; counter<dias.number ; counter++)
      if((dias.window[counter]->status==WINDOW_NMOPENED)||(dias.window[counter]->status==WINDOW_NMICONIZED)||(dias.window[counter]->status==WINDOW_NMREDRAW))
        found=TRUE;
    if((!found)||(info->order==dias.number))
    {
      wind_get(DESK,WF_TOP,&xy[Y4],&xy[X1],&xy[X1],&xy[X1]);
      wind_set(info->ident,WF_TOP,info->ident,NOLL,NOLL,NOLL);
      if(info->attr&NAME)
        wind_title(info->ident,info->w_name);
      if(info->attr&INFO)
        wind_info(info->ident,info->w_info);
#ifdef WINTEXT
      if(info->type==TYPE_TEXT)
      {
        convert_text(info,FALSE);
        if(!convert_text(info,TRUE))
        {
          close_dialog(info);
          return(FALSE);
        }
        set_text_sliders(info);
        if(xy[H]/info->text->char_h>info->text->num_of_rows)
          wind_set(info->ident,WF_VSLSIZE,1000,0,0,0);
        else
          wind_set(info->ident,WF_VSLSIZE,(SLIDESIZE*xy[H]/info->text->char_h)/info->text->num_of_rows,0,0,0);
      }
      if(info->type==TYPE_LIST)
      {
        wind_set(info->ident,WF_VSLSIZE,SLIDESIZE/info->text->num_of_rows,0,0,0);
        set_text_sliders(info);
        set_mark(info,info->text->select,FALSE);
      }
#endif
      redraw_window(info);
    }
  }
  return(TRUE);
}
/***********************************************************************/
/* Rutin f�r att st�nga ett dialogf�nster                              */
/***********************************************************************/
void close_dialog(WIN_DIALOG *info)
{
  int    count1,count2,next,last,first_string;
  int    nonmodal=FALSE;
  WINDOW tmp;

  if(info!=NULL)
  {
    if(info->status!=WINDOW_CLOSED)
    {
      if((info->status==WINDOW_NMOPENED)||(info->status==WINDOW_NMICONIZED)||(info->status==WINDOW_NMREDRAW))
      {
        dias.nonmodal--;
        nonmodal=TRUE;
      }
      wind_close(info->ident);
      wind_delete(info->ident);
      info->status=WINDOW_CLOSED;
	
      for(count1=NOLL, count2=NOLL; count1<dias.number; count1++)
        if(dias.window[count1]->ident!=info->ident)
        {
          tmp.window[count2]=dias.window[count1];
          tmp.diainfo[count2]=dias.diainfo[count1];
          count2++;
        }
      dias.number--;
      for(count1=0;count1<dias.number;count1++)
      {
        dias.window[count1]=tmp.window[count1];
        dias.diainfo[count1]=tmp.diainfo[count1];
      }
      info->ident=FAIL;
#ifdef WINTEXT
      if((info->type==TYPE_TEXT)||(info->type==TYPE_LIST))
      {
        convert_text(info,FALSE);
        v_clsvwk(info->text->graf_id);
      }
#endif
    }
  }
  else
  {
    for(count1=NOLL;count1<dias.number;count1++)
    {
      info=dias.window[count1];
      if(info->status!=WINDOW_CLOSED)
      {
        wind_close(info->ident);
        wind_delete(info->ident);
        info->ident=FAIL;
        info->status=WINDOW_CLOSED;
#ifdef WINTEXT
        if((info->type==TYPE_TEXT)||(info->type==TYPE_LIST))
        {
          convert_text(info,FALSE);
          v_clsvwk(info->text->graf_id);
        }
#endif
      }
    }
    dias.number=NOLL;
    dias.nonmodal=NOLL;
  }
  if((dias.nonmodal==NOLL)&&(nonmodal))
  {
    next=last=ROOT;
    if(menutree[next].ob_head!=FAIL)
      next=menutree[next].ob_head;
    else
      next=menutree[next].ob_next;
    first_string=TRUE;
    do
    {
      if(menutree[next].ob_type==G_TITLE)
      {
        if (!(menutree[next].ob_state&SPECIAL))
        {
          menutree[next].ob_state&=~DISABLED;
        }
        menutree[next].ob_state&=~SPECIAL;
      }
      else if(menutree[next].ob_type==G_STRING)
      {
        if(first_string)
        {
          if (!(menutree[next].ob_state&SPECIAL))
            menutree[next].ob_state&=~DISABLED;
          menutree[next].ob_state&=~SPECIAL;
        }
        first_string=FALSE;
      }
      last=next;

      if(menutree[next].ob_head!=FAIL)
        next=menutree[next].ob_head;
      else 
      {
        next=menutree[next].ob_next;
        while(next<last)
        {
          last=next;
          if(last==FAIL)
            next=ROOT;
          else
            next=menutree[next].ob_next;
        }
      }
    }while(next!=ROOT);
    menu_bar(menutree, 1);
  }
}
/***********************************************************************/
/* Rutin f�r att g�mma ett f�nster tex f�re start av ett annat prog    */
/***********************************************************************/
void hide_dialog(WIN_DIALOG *info)
{
  int counter;
  
  if(info!=NULL)
  {
    if((info->status!=WINDOW_CLOSED)&&(info->status!=WINDOW_HIDDEN)&&(info->status!=WINDOW_IHIDDEN)&&
       (info->status!=WINDOW_NMOPENED)&&(info->status!=WINDOW_NMICONIZED)&&(info->status!=WINDOW_NMREDRAW))
    {
      if(info->status==WINDOW_OPENED)
        info->status=WINDOW_HIDDEN;
      else if(info->status==WINDOW_ICONIZED)
        info->status=WINDOW_IHIDDEN;
      else if(info->status==WINDOW_REDRAW)
        info->status=WINDOW_HIDDEN;
      wind_close(info->ident);
    }
  }
  else
  {
    for(counter=NOLL;counter<dias.number;counter++)
    {
      info=dias.window[counter];
      if((info->status!=WINDOW_CLOSED)&&(info->status!=WINDOW_HIDDEN)&&(info->status!=WINDOW_IHIDDEN)&&
         (info->status!=WINDOW_NMOPENED)&&(info->status!=WINDOW_NMICONIZED)&&(info->status!=WINDOW_NMREDRAW))
      {
        if(info->status==WINDOW_OPENED)
          info->status=WINDOW_HIDDEN;
        else if(info->status==WINDOW_ICONIZED)
          info->status=WINDOW_IHIDDEN;
        else if(info->status==WINDOW_REDRAW)
          info->status=WINDOW_HIDDEN;
        wind_close(info->ident);
      }
    }
  }
}
/***********************************************************************/
/* rutin f�r at plocka fram ett g�mt f�nster                           */
/***********************************************************************/
void unhide_dialog(WIN_DIALOG *info)
{
  int counter;
  
  if(info!=NULL)
  {
    if(info->status==WINDOW_HIDDEN)
    {
      if(info->type==TYPE_DIALOG)
      {
        if(info->dialog!=NULL)
        {
          info->status=WINDOW_REDRAW;
          wind_open(info->ident, -1, -1, -1, -1);
        }
        else
        {
          info->status=WINDOW_OPENED;
          wind_open(info->ident,-1,-1,-1,-1);
        }
      }
    }
    else if(info->status==WINDOW_IHIDDEN)
    {
    }
  }
  else
  {
    for(counter=NOLL;counter<dias.number;counter++)
    {
      info=dias.window[counter];
      if(info->status==WINDOW_HIDDEN)
      {
        info->status=WINDOW_OPENED;
        if(info->type==TYPE_DIALOG)
        {
          info->status=WINDOW_REDRAW;
          if(info->dialog!=NULL)
          {
            wind_open(info->ident, -1, -1, -1, -1);
          }
          else
          {
            info->status=WINDOW_OPENED;
            wind_open(info->ident,-1,-1,-1,-1);
          }
        }
      }
      else if(info->status==WINDOW_IHIDDEN)
      {
      
      }
    }
  }
}
/***********************************************************************/
/* rutin f�r att rita om ett f�nster                                   */
/***********************************************************************/
void redraw_window(WIN_DIALOG *info)
{
  int counter;

  if(info!=NULL)
  {
    if((info->type==TYPE_DIALOG)&&(info->dialog!=NULL))
      update_dialog(info, NULL);
#ifdef WINTEXT
    else if(((info->type==TYPE_TEXT)||(info->type==TYPE_LIST))&&(info->text!=NULL))
      update_text(info, NULL);
#endif
  }
  else
  {
    for(counter=NOLL;counter<dias.number;counter++)
    {
      info=dias.window[counter];
      if((info->type==TYPE_DIALOG)&&(info->dialog!=NULL))
        update_dialog(info, NULL);
#ifdef WINTEXT
      else if(((info->type==TYPE_TEXT)||(info->type==TYPE_LIST))&&(info->text!=NULL))
        update_text(info, NULL);
#endif
    }
  }
}
/***********************************************************************/
/* Rutin f�r att st�lla in Timern OBS, endast 1 timer i taget          */
/***********************************************************************/
void set_timer(long time)
{
  timer=time;
}
/***********************************************************************/
/* Rutin f�r att hantera dialogf�nstrena och menyraden                 */
/***********************************************************************/
RESULT form_dialog()
{
  evnt.end=FALSE;
  do
  {
    if(evnt.check==NOLL)
    {
      if(timer>=NOLL)
        evnt.check=evnt_multi(MU_KEYBD|MU_BUTTON|MU_MESAG|MU_TIMER,258,MO_LEFT|MO_RIGHT,NOLL,NOLL,NOLL,NOLL,NOLL,NOLL,NOLL,NOLL,NOLL,NOLL,NOLL,evnt.data,timer&65535,timer>>16,&evnt.mo_x,&evnt.mo_y,&evnt.mo_b,&evnt.k_s,&evnt.key,&evnt.m_r);
      else
        evnt.check=evnt_multi(MU_KEYBD|MU_BUTTON|MU_MESAG,258,MO_LEFT|MO_RIGHT,NOLL,NOLL,NOLL,NOLL,NOLL,NOLL,NOLL,NOLL,NOLL,NOLL,NOLL,evnt.data,NOLL,NOLL,&evnt.mo_x,&evnt.mo_y,&evnt.mo_b,&evnt.k_s,&evnt.key,&evnt.m_r);
    }
    wind_get(DESK,WF_TOP,&(tebax.window),&evnt.w2,&evnt.w3,&evnt.w4);
    evnt.active=FAIL;
    for(evnt.count=0;evnt.count<dias.number;evnt.count++)
      if(tebax.window==dias.window[evnt.count]->ident)
        evnt.active=evnt.count;
    if(evnt.check&MU_MESAG)
    {
      fix_message(&evnt);
      evnt.check&=~MU_MESAG;
    }
    else if(evnt.check&MU_TIMER)
    {
      tebax.type=TIMER_EXIT;
      evnt.end=TRUE;
      evnt.check&=~MU_TIMER;
    }
    else if(evnt.check&MU_KEYBD)
    {
      fix_key_clicked();
      evnt.check&=~MU_KEYBD;
    }
     else if(evnt.check&MU_BUTTON)
    {
      fix_button_pressed();
      evnt.check&=~MU_BUTTON;
    }

    if(evnt.end)
    {
      return(tebax);
    }
  }while(TRUE);
}

/***********************************************************************/
/* Rutin f�r att uppdatera en del av ett f�nster                       */
/***********************************************************************/
void update_dialog(WIN_DIALOG *info, short xy[XYWH])
{
  short w[4];
  short t[4];
  int temp_window,temp_counter;
  if((info->status==WINDOW_HIDDEN)||(info->status==WINDOW_IHIDDEN))
    return;
  
  wind_update(BEG_UPDATE);
  wind_get(info->ident,WF_FIRSTXYWH,&w[X],&w[Y],&w[W],&w[H]);
  while((w[W]!=0)&&(w[H]!=0))
  {
    w[W]=(short)(w[W]+w[X]);	
    w[H]=(short)(w[Y]+w[H]);
    if(xy!=NULL)
    {
      t[X]=xy[X];
      t[Y]=xy[Y];
      t[W]=(short)(t[X]+xy[W]);
      t[H]=(short)(t[Y]+xy[H]);
    }
    else if((info->status==WINDOW_ICONIZED)||(info->status==WINDOW_NMICONIZED))
    {
      t[X]=info->i_x;
      t[Y]=info->i_y;
      t[W]=(short)(t[X]+info->icondata[ROOT].ob_width);
      t[H]=(short)(t[Y]+info->icondata[ROOT].ob_height);
    }
    else
    {
      t[X]=info->xy[X];
      t[Y]=info->xy[Y];
      t[W]=(short)(t[X]+info->xy[W]);
      t[H]=(short)(t[Y]+info->xy[H]);
    }
    if(t[X]<w[X])
      t[X]=w[X];
    if(t[Y]<w[Y])
      t[Y]=w[Y];
    if(t[W]>w[W])
      t[W]=w[W];
    if(t[H]>w[H])
      t[H]=w[H];
    if((t[W]>t[X])&&(t[H]>t[Y]))
    {
      if((info->status==WINDOW_ICONIZED)||(info->status==WINDOW_NMICONIZED))
      {
        info->icondata[ROOT].ob_x=info->i_x;
        info->icondata[ROOT].ob_y=info->i_y;
        objc_draw(info->icondata,ROOT,MAX_DEPTH,t[X],t[Y],t[W]-t[X],t[H]-t[Y]);
      }
      else if((info->status==WINDOW_OPENED)||(info->status==WINDOW_NMOPENED))
        objc_draw(info->dialog,ROOT,MAX_DEPTH,t[X],t[Y],t[W]-t[X],t[H]-t[Y]);
      else if(info->status==WINDOW_REDRAW)
      {
        info->status=WINDOW_OPENED;
      }
      else if(info->status==WINDOW_NMREDRAW)
        info->status=WINDOW_NMOPENED;
      temp_window=FAIL;
      for(temp_counter=0;temp_counter<dias.number;temp_counter++)
        if(info->ident==dias.window[temp_counter]->ident)
          temp_window=temp_counter;
      if(dias.diainfo[temp_window].text_object!=ROOT)
        objc_edit(info->dialog,dias.diainfo[temp_window].text_object,0,&dias.diainfo[temp_window].text_pos,ED_INIT);
       
    }
    wind_get(info->ident,WF_NEXTXYWH,&w[X],&w[Y],&w[W],&w[H]);
  }
  wind_update(END_UPDATE);
}
/***********************************************************************/
/* Rutin f�r att ikonisera ett �ppnat dialogf�nster                    */
/***********************************************************************/
void iconify_window(WIN_DIALOG *info)
{
/*
  if(((info->status==WINDOW_OPENED)||(info->status==WINDOW_NMOPENED)) && info->icondata!=NULL)
  {
    wind_set(info->ident,WF_ICONIFY,-1,-1,-1,-1);
    wind_get(info->ident,WF_WORKXYWH,(short *)&(info->i_x),(short *)&(info->i_y),&w,&h);
    
    if(info->status==WINDOW_OPENED)
      info->status=WINDOW_ICONIZED;
    else if(info->status==WINDOW_NMOPENED)
      info->status=WINDOW_NMICONIZED;
  }
*/

}

/***********************************************************************/
/* Rutin f�r att �ppna ett ikoniserat dialogf�nster                    */
/***********************************************************************/
void uniconify_window(WIN_DIALOG *info)
{
/*
  if((info->status==WINDOW_ICONIZED)||(info->status==WINDOW_NMICONIZED))
  {
    if(info->type=TYPE_DIALOG)
      wind_set(info->ident,WF_UNICONIFY,-1,-1,-1,-1);
#ifdef WINTEXT
    else if((info->type=TYPE_TEXT)||(info->type=TYPE_LIST))
      wind_set(info->ident,WF_UNICONIFY,-1,-1,-1,-1);
#endif
    if(info->status==WINDOW_ICONIZED)
      info->status=WINDOW_OPENED;
    else if(info->status==WINDOW_NMICONIZED)
      info->status=WINDOW_NMOPENED;
  }
*/
}


/***********************************************************************/
/* �ndrar Status och flaggor p� olika Objekt.                          */
/***********************************************************************/
void button(WIN_DIALOG *info, int object, int type ,short data,int draw)
{
  OBJECT *dialog;
  int temp,head,window=FAIL;

  if(info->type==TYPE_DIALOG)
    dialog=info->dialog;
#ifdef WINTEXT
  else if((info->type==TYPE_TEXT)||(info->type=TYPE_LIST))
    dialog=info->text->dialog;
#endif
  for(temp=NOLL;temp<dias.number;temp++)
  {
    if(info->ident==dias.window[temp]->ident)
      window=temp;
  }
  switch(type)
  {
    case SET_STATE:
      ((OBJECT *)dialog)[object].ob_state|=data;
      if((((OBJECT *)dialog)[object].ob_flags&RBUTTON)&&(data|SELECTED))
      {
        temp=((OBJECT *)dialog)[object].ob_next;
        head=FALSE;
        do
        {
          if((((OBJECT *)dialog)[temp].ob_flags&RBUTTON)&&(((OBJECT *)dialog)[temp].ob_state&SELECTED))
          {
            ((OBJECT *)dialog)[temp].ob_state&=~SELECTED;
            if((info->status==WINDOW_OPENED)||(info->status==WINDOW_NMOPENED))
            {
              b_place(dialog,temp,xy);
              if(info->type==TYPE_DIALOG)
                update_dialog(info, xy);
#ifdef WINTEXT
              else if((info->type==TYPE_TEXT)||(info->type==TYPE_LIST))
                update_text(info, xy);
#endif
            }
          }
          if((temp<object)&&(!head))
          {
            head=TRUE, temp=((OBJECT *)dialog)[temp].ob_head;
          }
          else
          {
            temp=((OBJECT *)dialog)[temp].ob_next;
          }
        }while(temp!=object);
      }
      if(((((OBJECT *)dialog)[object].ob_type==G_FTEXT)||(((OBJECT *)dialog)[object].ob_type==G_FBOXTEXT))&&(data|DISABLED)&&(window!=FAIL))
      {
        if(dias.diainfo[window].text_object==object)
        {
          dias.diainfo[window].text_object=search_for_text(dialog,ROOT);
        }
      }
      break;
    case CLEAR_STATE:
      ((OBJECT *)dialog)[object].ob_state&=~data;
      if(((((OBJECT *)dialog)[object].ob_type==G_FTEXT)||(((OBJECT *)dialog)[object].ob_type==G_FBOXTEXT))&&(((OBJECT *)dialog)[object].ob_flags&EDITABLE)&&(data|DISABLED)&&(window!=FAIL))
      {
        if(dias.diainfo[window].text_object==FAIL)
          dias.diainfo[window].text_object=object;
      } 
      break;
    case CHANGE_STATE:
      if(((OBJECT *)dialog)[object].ob_state&data)
        button(info,object, CLEAR_STATE,data,draw);
      else
        button(info,object, SET_STATE,data,draw);
      draw=FALSE;
      break;
    case SET_FLAGS:
      if(((OBJECT *)dialog)[object].ob_flags&HIDETREE)
        draw=FALSE;

      ((OBJECT *)dialog)[object].ob_flags|=data;

      if((data&HIDETREE)&&(window!=FAIL))
      {
        temp=((OBJECT *)dialog)[object].ob_next;
        if(object<=dias.diainfo[window].text_object<=temp)
          dias.diainfo[window].text_object=search_for_text(dialog,ROOT);
      }
      if(((((OBJECT *)dialog)[object].ob_type==G_FTEXT)||(((OBJECT *)dialog)[object].ob_type==G_FBOXTEXT))&&(data|EDITABLE)&&(window!=FAIL))
      {
        if(dias.diainfo[window].text_object==FAIL)
        {
          dias.diainfo[window].text_object=object;
        }
      }
      break;
    case CLEAR_FLAGS:
      ((OBJECT *)dialog)[object].ob_flags&=~data;
      if((data&HIDETREE)&&(window!=FAIL))
        if(dias.diainfo[window].text_object==FAIL)
          dias.diainfo[window].text_object=search_for_text(dialog,ROOT);
      if(((((OBJECT *)dialog)[object].ob_type==G_FTEXT)||(((OBJECT *)dialog)[object].ob_type==G_FBOXTEXT))&&(data|EDITABLE)&&(window!=FAIL))
        if(dias.diainfo[window].text_object==object)
          dias.diainfo[window].text_object=search_for_text(dialog,object);
      break;
    case CHANGE_FLAGS:
      if(((OBJECT *)dialog)[object].ob_flags&data)
        button(info,object, CLEAR_FLAGS,data,draw);
      else
        button(info,object, SET_FLAGS,data,draw);
      draw=FALSE;
      break;
    case UPDATE:
      break;
  }
  if(((info->status==WINDOW_OPENED)||(info->status==WINDOW_NMOPENED))&&(draw))
  {
    b_place(dialog,object,xy);
    if(info->type==TYPE_DIALOG)
      update_dialog(info, xy);
#ifdef WINTEXT
    else if((info->type==TYPE_TEXT)||(info->type==TYPE_LIST))
      update_text(info, xy);
#endif
  }
  else
  {
  }
}
/***********************************************************************/
/*                                                                     */
/***********************************************************************/
int search_for_text(OBJECT *tree,int object)
{
  int next=object, last=object;
  
  if((tree[next].ob_head!=FAIL)&&((tree[next].ob_flags&HIDETREE)==NOLL))
    next=tree[next].ob_head;
  else
  {
    next=tree[next].ob_next;
    while(next<last)
    {
      last=next;
      if(last==FAIL)
        next=ROOT;
      else
        next=tree[next].ob_next;
    }
  }  
  do
  {
    if(((tree[next].ob_type==G_FTEXT)||(tree[next].ob_type==G_FBOXTEXT))&&
       ((tree[next].ob_flags&EDITABLE)&&(!(tree[next].ob_state&DISABLED))&&(!(tree[next].ob_flags&HIDETREE))))
      return(next);
    last=next;
    if((tree[next].ob_head!=FAIL)&&(!(tree[next].ob_flags&HIDETREE)))
      next=tree[next].ob_head;
    else 
    {
      next=tree[next].ob_next;
      while(next<last)
      {
        last=next;
        if(last==FAIL)
          next=ROOT;
        else
          next=tree[next].ob_next;
      }
    }
  }while(next!=object);
  return(FAIL);
}

/***********************************************************************/
/***********************************************************************/
void place_cursor(OBJECT *tree,int text_object,int cur_place)
{
}
/***********************************************************************/
/***********************************************************************/
int search_for_flags(OBJECT *tree,int object,int flags)
{
  int next=object, last=object;

  if(tree[next].ob_head!=FAIL)
    next=tree[next].ob_head;
  else
    next=tree[next].ob_next;
  
  do
  {
    if(tree[next].ob_flags&flags)
      return(next);
   
    last=next;
    if(tree[next].ob_head!=FAIL)
      next=tree[next].ob_head;
    else 
    {
      next=tree[next].ob_next;
      while(next<last)
      {
        last=next;
        if(last==FAIL)
          next=ROOT;
        else
          next=tree[next].ob_next;
      }
    }
  }while(next!=object);
  return(FAIL);
}

/***********************************************************************/
/* Rutin f�r att kopiera en text till ett Ted-objekt.                  */
/***********************************************************************/
void str2ted(OBJECT *tree, int object, char *text)
{
  if((tree[object].ob_type==G_FTEXT)||(tree[object].ob_type==G_FBOXTEXT))
    strncpy(tree[object].tedinfo->te_ptext,text,strlen(((TEDINFO *)tree[object].ob_spec)->te_pvalid));
  else if((tree[object].ob_type==G_TEXT)||(tree[object].ob_type==G_BOXTEXT))
    strcpy(tree[object].tedinfo->te_ptext,text);
  else if((tree[object].ob_type==G_STRING)||(tree[object].ob_type==G_BUTTON))
    strcpy(tree[object].string,text);
}

/***********************************************************************/
/* Rutin f�r att kopiera en text fr�n ett Ted-objekt                   */
/***********************************************************************/
void ted2str(OBJECT *tree, int object, char *text)
{
  if((tree[object].ob_type==G_FTEXT)||(tree[object].ob_type==G_FBOXTEXT)||(tree[object].ob_type==G_TEXT)||(tree[object].ob_type==G_BOXTEXT))
    strcpy(text,tree[object].tedinfo->te_ptext);
  else if((tree[object].ob_type==G_STRING)||(tree[object].ob_type==G_BUTTON))
    strcpy(text,tree[object].string);
}
/***********************************************************************/
/* Rutin f�r att kopiera ett tal fr�n ett Ted-objekt                   */
/***********************************************************************/
int ted2int(OBJECT *tree, int object)
{
  if((tree[object].ob_type==G_FTEXT)||(tree[object].ob_type==G_FBOXTEXT)||(tree[object].ob_type==G_TEXT)||(tree[object].ob_type==G_BOXTEXT))
    return (atoi(tree[object].tedinfo->te_ptext));
  else if((tree[object].ob_type==G_STRING)||(tree[object].ob_type==G_BUTTON))
    return (atoi(tree[object].string));
}

/********************************************************************/
/* Rutin f�r att fixa alert-boxar                                   */
/********************************************************************/
int alertbox(int button, char *string)
{
//  alert_init(button,string);
//  getch(); 
//  alert_deinit(button,string);
  return(form_alert(button,string));
}
/********************************************************************/
/********************************************************************/
void alert_init(int button, char *string)
{
  int i;
  /*char temp[255],*dummy1,dummy2; */
  for(i=0; i<=11; i++)
    rsrc_obfix(ALERTBOX,i);
  //ALERTBOX[ALERT_ICON].ob_spec=ALERT_CICONS[ICON_1].ob_spec;
  form_center(ALERTBOX,&dummy,&dummy,&dummy,&dummy);
  wind_update(BEG_UPDATE);
  wind_update(BEG_MCTRL);
  form_dial(FMD_START,0,0,0,0,ALERTBOX[ROOT].ob_x-5,ALERTBOX[ROOT].ob_y-5,ALERTBOX[ROOT].ob_width+10,ALERTBOX[ROOT].ob_height+10);
  objc_draw(ALERTBOX,ROOT,MAX_DEPTH,screenx,screeny,screenw,screenh);
}
/********************************************************************/
/********************************************************************/
void alert_deinit(char *alert)
{
  form_dial(FMD_FINISH,0,0,0,0,ALERTBOX[ROOT].ob_x-5,ALERTBOX[ROOT].ob_y-5,ALERTBOX[ROOT].ob_width+10,ALERTBOX[ROOT].ob_height+10);
  wind_update(END_MCTRL);
  wind_update(END_UPDATE);

}
/********************************************************************/
/* rutin f�r att kolla placering och storlek p� ett objekt          */
/********************************************************************/
void b_place(OBJECT *dialog, int object, short xy[XYWH])
{
  int flags,state,type;
  
  objc_offset(dialog,object,&xy[X],&xy[Y]);
  type=dialog[object].ob_type;
  flags=dialog[object].ob_flags;
  state=dialog[object].ob_state;
  xy[W]=dialog[object].ob_width;
  xy[H]=dialog[object].ob_height;
  if(type==G_BUTTON)
  {
    xy[X]-=3;
    xy[Y]-=3;
    xy[W]+=6;
    xy[H]+=6;
  }
  else if(type=G_IMAGE)
  {
    xy[X]-=2;
    xy[Y]-=2;
    xy[W]+=4;
    xy[H]+=4;
  }
  
  if(flags&EXIT)
  {
    xy[X]--;
    xy[Y]--;
    xy[W]+=2;
    xy[H]+=2;
  }
  if(flags&DEFAULT)
  {
    xy[X]--;
    xy[Y]--;
    xy[W]+=2;
    xy[H]+=2;
  }
}

/********************************************************************/
/* rutin f�r att flytta 1 objekt i ett annat                        */
/********************************************************************/
void fix_drag(WIN_DIALOG *info)
{
  OBJECT *dialog;
  short p_xy[XYWH],c_xy[XYWH],x,y;
  short parent;
  button(info,evnt.d1,SET_STATE,SELECTED,TRUE);
  if(info->type=TYPE_DIALOG)
    dialog=info->dialog;
  else if((info->type==TYPE_TEXT)||(info->type==TYPE_LIST))
    dialog=info->text->dialog;
  parent=((OBJECT *)dialog)[evnt.d1].ob_next;
  while(parent>evnt.d1)
    parent=((OBJECT *)dialog)[evnt.d1].ob_next;
  if(parent==FAIL)
    return;
  b_place(dialog,parent,p_xy);
  b_place(dialog,evnt.d1,c_xy);
  graf_dragbox(c_xy[W],c_xy[H],c_xy[X],c_xy[Y]+1,p_xy[X],p_xy[Y],p_xy[W],p_xy[H],&x,&y);
  
  tebax.type=DIALOG_CLICKED;
  tebax.data[SVAR_OBJECT]=evnt.d1;
  tebax.data[SVAR_MOUSE_BUTTON]=evnt.m_r;
  tebax.data[SVAR_WINDOW_ID]=info->ident;
  evnt.end=TRUE;
  ((OBJECT*)dialog)[evnt.d1].ob_x=x-p_xy[X];
  ((OBJECT*)dialog)[evnt.d1].ob_y=y-p_xy[Y];
  button(info,evnt.d1,CLEAR_STATE,SELECTED,FALSE);
  button(info,parent,UPDATE,SELECTED,TRUE);
  return;
}
/********************************************************************/
/* rution f�r att kolla mus-knapps eventet                          */
/********************************************************************/
void fix_button_pressed()
{
  int check;
  WIN_DIALOG *info;
  if(evnt.active==FAIL)
  {
    tebax.type=BUTTON_CLICKED;
    tebax.data[SVAR_MOUSE_BUTTON]=evnt.mo_b;
    tebax.data[SVAR_MOUSE_X]=evnt.mo_x;
    tebax.data[SVAR_MOUSE_Y]=evnt.mo_y;
    tebax.data[SVAR_MOUSE_CLICKS]=evnt.m_r;
    evnt.end=TRUE;
    return;  
  }
  info=dias.window[evnt.active];
  if((dias.nonmodal>0)&&(info->order!=dias.number))
  {
    evnt.end=FALSE;
    return;
  }
  if((info->type==TYPE_DIALOG))
  {
    tebax.data[SVAR_MOUSE_BUTTON]=evnt.mo_b;
    tebax.data[SVAR_MOUSE_X]=evnt.mo_x;
    tebax.data[SVAR_MOUSE_Y]=evnt.mo_y;
    tebax.data[SVAR_MOUSE_CLICKS]=evnt.m_r;
    if(evnt.mo_b==MO_LEFT)
    {
      evnt.d1=objc_find(info->dialog,ROOT,MAX_DEPTH,evnt.mo_x,evnt.mo_y);
      if(evnt.d1>=0)
      {
        b_place(info->dialog,evnt.d1,evnt.xy);
        tebax.data[SVAR_MOUSE_X]=evnt.mo_x-evnt.xy[X];
        tebax.data[SVAR_MOUSE_Y]=evnt.mo_y-evnt.xy[Y];
        evnt.d2=((OBJECT*)info->dialog)[evnt.d1].ob_state;
        evnt.d3=((OBJECT*)info->dialog)[evnt.d1].ob_flags;
        evnt.d4=((OBJECT*)info->dialog)[evnt.d1].ob_type;
        if(!(evnt.d2&DISABLED))
        {
          if(((evnt.d4==G_FTEXT)||(evnt.d4==G_FBOXTEXT))&&(evnt.d3&EDITABLE))
          {
            objc_edit(info->dialog,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_END);
            dias.diainfo[evnt.active].text_object=evnt.d1;
            dias.diainfo[evnt.active].text_pos=strlen(((TEDINFO*)((OBJECT*)info->dialog)[evnt.d1].ob_spec)->te_ptext);
            objc_edit(info->dialog,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_INIT);
          }
          else if(evnt.d4==GE_DRAGBOX)
          {
            fix_drag(info);
            return;
          }
          if(evnt.d3&SELECTABLE)
          {
            if((evnt.d4=G_BOX)&&((char)(((long)info->dialog[evnt.d1].ob_spec>>24)&255)=='X'))
              button(info,evnt.d1,CHANGE_STATE,CROSSED,TRUE);
            else if((evnt.d4=G_BOX)&&((char)(((long)info->dialog[evnt.d1].ob_spec>>24)&255)=='Y'))
              button(info,evnt.d1,CHANGE_STATE,CHECKED,TRUE);
            else
              button(info,evnt.d1,CHANGE_STATE,SELECTED,TRUE);
          }
          if(evnt.d3&TOUCHEXIT)
          {
            tebax.type=DIALOG_CLICKED;
            tebax.data[SVAR_OBJECT]=evnt.d1;
            tebax.data[SVAR_WINDOW_ID]=info->ident;
            evnt.end=TRUE;
            return;
          }
          else if((evnt.d3&EXIT2)&&(evnt.m_r==2))
          {
            tebax.type=DIALOG_CLICKED;
            tebax.data[SVAR_OBJECT]=evnt.d1;
            tebax.data[SVAR_WINDOW_ID]=info->ident;
            evnt.end=TRUE;
            return;
          }
          else
          {
            b_place(info->dialog,evnt.d1,evnt.xy);
            check=evnt_multi(MU_BUTTON|MU_M1,1,MO_LEFT,MO_NO_BUTT,1,evnt.xy[X],evnt.xy[Y],evnt.xy[W],evnt.xy[H],0,0,0,0,0,0,0,0,&evnt.mo_x,&evnt.mo_y,&evnt.mo_b,&evnt.k_s,&evnt.key,&evnt.m_r);
            if(check&MU_BUTTON)
            {
              if(evnt.d3&EXIT)
              {
                tebax.type=DIALOG_CLICKED;
                tebax.data[SVAR_OBJECT]=evnt.d1;
                tebax.data[SVAR_WINDOW_ID]=info->ident;
                evnt.end=TRUE;
                return;
              }
            }
            else if((check&MU_M1)&&(evnt.d3&SELECTABLE))
              button(dias.window[evnt.active],evnt.d1,CLEAR_STATE,SELECTED,TRUE);
            check&=~MU_M1;
            evnt.check|=check;
          }
        }
      }
      else
      {
        tebax.type=BUTTON_CLICKED;
        evnt.end=TRUE;
        return;
      }
    }
    else
    {
      evnt.d1=objc_find(info->dialog,ROOT,MAX_DEPTH,evnt.mo_x,evnt.mo_y);
      if(evnt.d1>=0)
      {
        tebax.data[SVAR_MOUSE_CLICKS]=evnt.m_r;
        evnt.d2=((OBJECT*)info->dialog)[evnt.d1].ob_state;
        evnt.d3=((OBJECT*)info->dialog)[evnt.d1].ob_flags;
        evnt.d4=((OBJECT*)info->dialog)[evnt.d1].ob_type;
        if(!(evnt.d2&DISABLED))
        {
          if(((evnt.d4==G_FTEXT)||(evnt.d4==G_FBOXTEXT))&&(evnt.d3&EDITABLE))
          {
#ifdef WINPOPUP
            switch(freepopup("Select Action|Cut text|Copy text|Clear text|Paste text",-1,evnt.mo_x,evnt.mo_y,d_temp))
            {
              case 0:
                objc_edit(info->dialog,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_END);
                tedcut(info->dialog,evnt.d1);
                objc_edit(info->dialog,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_INIT);
                break;
              case 1:
                objc_edit(info->dialog,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_END);
                tedcopy(info->dialog,evnt.d1);
                objc_edit(info->dialog,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_INIT);
                break;
              case 2:
                objc_edit(info->dialog,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_END);
                ((TEDINFO *)info->dialog[evnt.d1].ob_spec)->te_ptext[NOLL]=NOLL;
                objc_draw(info->dialog,evnt.d1,1,screenx,screeny,screenw,screenh);
                dias.diainfo[evnt.active].text_pos=NOLL;
                objc_edit(info->dialog,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_INIT);
                break;
              case 3:
                objc_edit(info->dialog,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_END);
                tedpaste(info->dialog,evnt.d1);
                objc_edit(info->dialog,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_INIT);
                break;
            }
#endif
          }
        }
      }
    }
  }
#ifdef WINTEXT
  else if(info->type==TYPE_TEXT || info->type==TYPE_LIST)
  {
    WIN_TEXT *text =info->text;
    if(text==NOLL)
      return;
    if(text->num_of_rows==NOLL)
      return;
    evnt.w1=evnt.mo_y-info->xy[Y];
    evnt.w2=evnt.mo_x-info->xy[X];
    if(text->dialog!=NOLL)
      evnt.w1-=((OBJECT *)text->dialog)[ROOT].ob_height;
    if(evnt.w1<0)
    {
      tebax.data[SVAR_MOUSE_BUTTON]=evnt.mo_b;
      tebax.data[SVAR_MOUSE_X]=evnt.mo_x;
      tebax.data[SVAR_MOUSE_Y]=evnt.mo_y;
      tebax.data[SVAR_MOUSE_CLICKS]=evnt.m_r;
      if(evnt.mo_b==MO_LEFT)
      {
        evnt.d1=objc_find(text->dialog,ROOT,MAX_DEPTH,evnt.mo_x,evnt.mo_y);
        if(evnt.d1>=0)
        {
          b_place(text->dialog,evnt.d1,evnt.xy);
          tebax.data[SVAR_MOUSE_X]=evnt.mo_x-evnt.xy[X];
          tebax.data[SVAR_MOUSE_Y]=evnt.mo_y-evnt.xy[Y];
          evnt.d2=((OBJECT*)text->dialog)[evnt.d1].ob_state;
          evnt.d3=((OBJECT*)text->dialog)[evnt.d1].ob_flags;
          evnt.d4=((OBJECT*)text->dialog)[evnt.d1].ob_type;
          if(!(evnt.d2&DISABLED))
          {
            if(((evnt.d4==G_FTEXT)||(evnt.d4==G_FBOXTEXT))&&(evnt.d3&EDITABLE))
            {
              objc_edit(text->dialog,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_END);
              dias.diainfo[evnt.active].text_object=evnt.d1;
              dias.diainfo[evnt.active].text_pos=strlen(((TEDINFO*)((OBJECT*)text->dialog)[evnt.d1].ob_spec)->te_ptext);
              objc_edit(text->dialog,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_INIT);
            }
            else if(evnt.d4==GE_DRAGBOX)
            {
              fix_drag(info);
              return;
            }
            if(evnt.d3&SELECTABLE)
            {
              if((evnt.d4=G_BOX)&&((char)(((long)text->dialog[evnt.d1].ob_spec>>24)&255)=='X'))
              {
                button(info,evnt.d1,CHANGE_STATE,CROSSED,TRUE);
              }
              else if((evnt.d4=G_BOX)&&((char)(((long)text->dialog[evnt.d1].ob_spec>>24)&255)=='Y'))
              {
                button(info,evnt.d1,CHANGE_STATE,CHECKED,TRUE);
              }
              else
              {
                button(info,evnt.d1,CHANGE_STATE,SELECTED,TRUE);
              }
            }
            if(evnt.d3&TOUCHEXIT)
            {
              tebax.type=DIALOG_CLICKED;
              tebax.data[SVAR_OBJECT]=evnt.d1;
              tebax.data[SVAR_WINDOW_ID]=info->ident;
              evnt.end=TRUE;
              return;
            }
            else if((evnt.d3&EXIT2)&&(evnt.m_r==2))
            {
              tebax.type=DIALOG_CLICKED;
              tebax.data[SVAR_OBJECT]=evnt.d1;
              tebax.data[SVAR_WINDOW_ID]=info->ident;
              evnt.end=TRUE;
              return;
            }
            else
            {
              b_place(text->dialog,evnt.d1,evnt.xy);
              check=evnt_multi(MU_BUTTON|MU_M1,1,MO_LEFT,MO_NO_BUTT,1,evnt.xy[X],evnt.xy[Y],evnt.xy[W],evnt.xy[H],0,0,0,0,0,0,0,0,&evnt.mo_x,&evnt.mo_y,&evnt.mo_b,&evnt.k_s,&evnt.key,&evnt.m_r);
              if(check&MU_BUTTON)
              {
                if(evnt.d3&EXIT)
                {
                  tebax.type=DIALOG_CLICKED;
                  tebax.data[SVAR_OBJECT]=evnt.d1;
                  tebax.data[SVAR_WINDOW_ID]=info->ident;
                  evnt.end=TRUE;
                  return;
                }
              }
              else if((check&MU_M1)&&(evnt.d3&SELECTABLE))
              {
                button(dias.window[evnt.active],evnt.d1,CLEAR_STATE,SELECTED,TRUE);
              }
              check&=~MU_M1;
              evnt.check|=check;
            }
          }
        }
        else
        {
          tebax.type=BUTTON_CLICKED;
          evnt.end=TRUE;
          return;
        }
      }
      else
      {
        evnt.d1=objc_find(text->dialog,ROOT,MAX_DEPTH,evnt.mo_x,evnt.mo_y);
        if(evnt.d1>=0)
        {
          evnt.d2=((OBJECT*)text->dialog)[evnt.d1].ob_state;
          evnt.d3=((OBJECT*)text->dialog)[evnt.d1].ob_flags;
          evnt.d4=((OBJECT*)text->dialog)[evnt.d1].ob_type;
          if(!(evnt.d2&DISABLED))
          {
            if(((evnt.d4==G_FTEXT)||(evnt.d4==G_FBOXTEXT))&&(evnt.d3&EDITABLE))
            {
  #ifdef WINPOPUP
              switch(freepopup("Select Action|Cut text|Copy text|Clear text|Paste text",-1,evnt.mo_x,evnt.mo_y,d_temp))
              {
                case 0:
                  objc_edit(text->dialog,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_END);
                  tedcut(text->dialog,evnt.d1);
                  objc_edit(text->dialog,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_INIT);
                  break;
                case 1:
                  objc_edit(text->dialog,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_END);
                  tedcopy(text->dialog,evnt.d1);
                  objc_edit(text->dialog,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_INIT);
                  break;
                case 2:
                  objc_edit(text->dialog,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_END);
                  ((TEDINFO *)text->dialog[evnt.d1].ob_spec)->te_ptext[NOLL]=NOLL;
                  objc_draw(text->dialog,evnt.d1,1,screenx,screeny,screenw,screenh);
                  dias.diainfo[evnt.active].text_pos=NOLL;
                  objc_edit(text->dialog,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_INIT);
                  break;
                case 3:
                  objc_edit(text->dialog,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_END);
                  tedpaste(text->dialog,evnt.d1);
                  objc_edit(text->dialog,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_INIT);
                  break;
              }
#endif
            }
          }
        }
      }
    }
    else if(info->type==TYPE_LIST)
    {
      evnt.w2=text->select;
      text->select=text->offset_y+evnt.w1/text->char_h;
      if(text->select>=text->num_of_rows)
        text->select=text->num_of_rows-1;
      if(evnt.w2!=text->select)
      {
        list_mark(info,evnt.w2,FALSE);
        if(text->select>text->offset_y+info->xy[H]/text->char_h-1)
        {
          if(text->offset_y<text->num_of_rows-(2*(info->xy[H]/text->char_h)))
          {
            text->offset_y+=info->xy[H]/text->char_h;
            update_text(info,NULL);
          }
          else if(text->offset_y<text->num_of_rows-(info->xy[H]/text->char_h))
          {
            text->offset_y=text->num_of_rows-info->xy[H]/text->char_h;
            update_text(info,NULL);
          }
        }
        else
          list_mark(info,text->select,FALSE);
      }
      tebax.type=TEXT_CLICKED;
      tebax.data[SVAR_WINDOW_ID]=info->ident;
      tebax.data[SVAR_OBJECT]=text->select;
      tebax.data[SVAR_MOUSE_BUTTON]=evnt.mo_b;
      tebax.data[SVAR_MOUSE_X]=evnt.mo_x;
      tebax.data[SVAR_MOUSE_Y]=evnt.mo_y;
      tebax.data[SVAR_MOUSE_CLICKS]=evnt.m_r;
      evnt.end=TRUE;
    }
    else if(info->type==TYPE_TEXT)
    {
      if(evnt.m_r==1 && evnt.mo_b==MO_LEFT && evnt.k_s==0)
      {
//        copy_text(info,evnt.w2,evnt.w1);
      }
      else if(evnt.m_r==1 && evnt.mo_b==MO_LEFT && evnt.k_s==K_CTRL)
      {
//        copy_textarea(info,evnt.w2,evnt.w1);
      }
      else if(evnt.m_r==1 && evnt.mo_b==MO_LEFT && evnt.k_s==K_ALT)
      {
        copy_word(info,evnt.w2,evnt.w1);
      }   
      else if(evnt.m_r==2 && evnt.mo_b==MO_LEFT)
      {
        char *start=0,*end=0;
        WIN_TEXT *text =info->text;
  
        text->mark.start_row=evnt.w1/text->char_h+text->offset_y;
        text->mark.start_col=evnt.w2/text->char_w+text->offset_x;
        text->mark.end_row=text->mark.start_row;
        text->mark.end_col=text->mark.start_col;
        start=text->lineinfo[text->mark.start_row].line+text->mark.start_col;
        end=start;
        while ((*start!='\n')&&(*start!='\r')&&(*start!=' ')&&(*start!='\t')&&(*start!='\0')&&(start>text->textstart))
        {
          start--;
          text->mark.start_col--;
        }
        start++;
        text->mark.start_col++;
        while ((*end!='\n')&&(*end!='\r')&&(*end!=' ')&&(*end!='\t')&&(*end!='\0')&&(end<text->textend))
        {
          end++;
          text->mark.end_col++;
        }
        end--;
        text->mark.start_col--;
        tebax.type=TEXT_SELECTED;
        tebax.data[SVAR_TEXT_START_HI]=(short) start >> 16;
        tebax.data[SVAR_TEXT_START_LO]=(short) start & 0xffff;
        tebax.data[SVAR_TEXT_END_HI]=(short) end >> 16;
        tebax.data[SVAR_TEXT_END_LO]=(short) end & 0xffff;
        tebax.data[SVAR_TEXT_LENGTH]=(short)(end-start);
        evnt.end=TRUE;
      }
      else
      {
        tebax.type=TEXT_CLICKED;
        tebax.data[SVAR_MOUSE_CLICKS]=evnt.m_r;
        tebax.data[SVAR_MOUSE_BUTTON]=evnt.mo_b;
        tebax.data[SVAR_WINDOW_ID]=info->ident;
        tebax.data[SVAR_OBJECT]=-1;
        tebax.data[SVAR_MOUSE_X]=evnt.w2;
        tebax.data[SVAR_MOUSE_Y]=evnt.w1;
        evnt.end=TRUE;
      }
    }
  }
#endif
  else
  {
    tebax.type=BUTTON_CLICKED;
    tebax.data[SVAR_MOUSE_BUTTON]=evnt.mo_b;
    tebax.data[SVAR_MOUSE_X]=evnt.mo_x;
    tebax.data[SVAR_MOUSE_Y]=evnt.mo_y;
    tebax.data[SVAR_MOUSE_CLICKS]=evnt.m_r;
    evnt.end=TRUE;
    return;
  }
}


/********************************************************************/
/* Kollar Tangent-tryck och utf�r de �tg�rder som beh�vs            */
/********************************************************************/
void fix_key_clicked()
{
  int    menuitem;
  char   key=evnt.key&255;
  char   scan=evnt.key/256;
  WIN_DIALOG *info;


  if(fix_key_dialog())
  {
    return;
  }
#ifdef WINTEXT
  else if(fix_key_text())
  {
    return;
  }
#endif
  else if (dias.nonmodal==NOLL)
  {
    if((menuitem=check_menu_shortkey(evnt.key,evnt.k_s))!=FAIL)
    {
      tebax.type=MENU_CLICKED;
      tebax.data[SVAR_OBJECT]=menuitem;
      tebax.data[SVAR_OBJECT_MENU_ROW]=FAIL;
      evnt.end=TRUE;
      return;
    }
  }

  if(evnt.active>FAIL)
  {
    info=dias.window[evnt.active];
    key=key_table[0][scan];
    switch(key)
    {
      case 'u':
      {
        if(evnt.k_s==K_CTRL)
        {
          if(evnt.active!=FAIL)
          {
            tebax.type=WINDOW_CLICKED;
            tebax.data[SVAR_WINDOW_MESSAGE]=WM_CLOSED;
            tebax.data[SVAR_WINDOW_ID]=info->ident;
            evnt.end=TRUE;
            return;
          }
        }
        break;
      }
      case 'w':
      {
        if(evnt.k_s==K_CTRL)
        {
          wind_set(info->ident,WF_BOTTOM,info->ident,0,0,0);
          return;
        }
        else if(evnt.k_s==K_CTRL|K_RSHIFT|K_LSHIFT)
        {
          wind_get(info->ident,WF_BOTTOM,&dummy,0,0,0);
          wind_set(dummy,WF_TOP,dummy,0,0,0);
          return;
        }
        break;
      }
    }
  }
  tebax.type=KEY_CLICKED;
  tebax.data[SVAR_KEY_VALUE]=evnt.key;
  tebax.data[SVAR_KEY_SHIFT]=evnt.k_s;
  if(evnt.active>FAIL)
    tebax.data[SVAR_WINDOW_ID]=info->ident;
  else
    tebax.data[SVAR_WINDOW_ID]=NOLL;
    
  evnt.end=TRUE;
}
/********************************************************************/
/* Inmatning i editerbara f�lt (Dialoger)                           */
/********************************************************************/
int fix_key_dialog()
{
  WIN_DIALOG *info;
  int    object;
  OBJECT *tree;
  char   key=evnt.key&255;
  char   scan=evnt.key/256;
  int    validlen,textlen;


  if(evnt.active==FAIL)
    return(FALSE);
  info=dias.window[evnt.active];
  if(info->type!=TYPE_DIALOG)
    return(FALSE);
  
  tree=info->dialog;
  if(tree==NOLL)
    return(FALSE);
  
  object=dias.diainfo[evnt.active].text_object;
  if(object==FAIL)
  {
    switch(key)
    {
      case '\r':
      case '\n':
      {
        object=search_for_flags(tree,ROOT,DEFAULT);
        if(object!=FAIL)
        {
          tebax.type=DIALOG_CLICKED;
          tebax.data[SVAR_OBJECT]=object;
          tebax.data[SVAR_MOUSE_BUTTON]=MO_LEFT;
          tebax.data[SVAR_WINDOW_ID]=info->ident;
          evnt.end=TRUE;
          return(TRUE);
        }
        break;
      }
    }
  }
  else if((evnt.key!=NOLL)&&(tree[object].ob_flags|EDITABLE))
  {
    validlen=strlen(((TEDINFO *)tree[object].ob_spec)->te_pvalid),
    textlen=strlen(((TEDINFO *)tree[object].ob_spec)->te_ptext);
    switch(key)
    {
      case '\r':
      case '\n':
      {
        dummy=object;
        object=search_for_text(tree,object);
        if(object<=dummy)
        {
          
          dummy=search_for_flags(tree,ROOT,DEFAULT);
          if(dummy!=FAIL)
          {
            tebax.type=DIALOG_CLICKED;
            tebax.data[SVAR_OBJECT]=dummy;
            tebax.data[SVAR_MOUSE_BUTTON]=MO_LEFT;
            tebax.data[SVAR_WINDOW_ID]=info->ident;
            evnt.end=TRUE;
            return(TRUE);
          }
        }
        objc_edit(tree,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_END);
        dias.diainfo[evnt.active].text_object=object;
        dias.diainfo[evnt.active].text_pos=strlen(((TEDINFO *)tree[object].ob_spec)->te_ptext);
        objc_edit(tree,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_INIT);
        return(TRUE);
        break;
      }
      case NU_BACKSPC:
      {
        if(textlen>0)
        {
          objc_edit(tree,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_END);
          ((TEDINFO *)tree[object].ob_spec)->te_ptext[--textlen]=0;
          objc_draw(tree,object,1,screenx,screeny,screenw,screenh);
          dias.diainfo[evnt.active].text_pos--;
          objc_edit(tree,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_INIT);
        }
        return(TRUE);
        break;
      }
      case NU_TAB:
      {
        dummy=object;
        objc_edit(tree,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_END);
        object=search_for_text(tree,object);
        dias.diainfo[evnt.active].text_object=object;
        dias.diainfo[evnt.active].text_pos=strlen(((TEDINFO *)tree[object].ob_spec)->te_ptext);
        objc_edit(tree,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_INIT);
        return(TRUE);
        break;
      }
      case NU_ESC:
      {
        objc_edit(tree,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_END);
        ((TEDINFO *)tree[object].ob_spec)->te_ptext[NOLL]=NOLL;
        objc_draw(tree,object,1,screenx,screeny,screenw,screenh);
        dias.diainfo[evnt.active].text_pos=NOLL;
        objc_edit(tree,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_INIT);
        return(TRUE);
        break;
      }
      default:
      {
        switch(scan)
        {
          case SC_CLR:
          {
            objc_edit(tree,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_END);
            ((TEDINFO *)tree[object].ob_spec)->te_ptext[NOLL]=NOLL;
            objc_draw(tree,object,1,screenx,screeny,screenw,screenh);
            dias.diainfo[evnt.active].text_pos=NOLL;
            objc_edit(tree,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_INIT);
            return(TRUE);
            break;
          }
          case SC_UP:
          {
            objc_edit(tree,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_END);
            dummy=object;
            object=search_for_text(tree,ROOT);
            while(object<dias.diainfo[evnt.active].text_object)
            {
              dummy=object;
              object=search_for_text(tree,object);
            }  
            dias.diainfo[evnt.active].text_object=dummy;
            if(dias.diainfo[evnt.active].text_pos>strlen(((TEDINFO *)tree[object].ob_spec)->te_ptext))
              dias.diainfo[evnt.active].text_pos=strlen(((TEDINFO *)tree[object].ob_spec)->te_ptext);
            objc_edit(tree,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_INIT);
            return(TRUE);
            break;
          }
          case SC_DOWN:
          {
            dummy=object;
            objc_edit(tree,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_END);
            object=search_for_text(tree,object);
            if(object<dummy)
              object=dummy;
            dias.diainfo[evnt.active].text_object=object;
            if(dias.diainfo[evnt.active].text_pos>strlen(((TEDINFO *)tree[object].ob_spec)->te_ptext))
              dias.diainfo[evnt.active].text_pos=strlen(((TEDINFO *)tree[object].ob_spec)->te_ptext);
            objc_edit(tree,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_INIT);
            return(TRUE);
            break;
          }
          case SC_LEFT:
          {
            objc_edit(tree,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_END);
            if(dias.diainfo[evnt.active].text_pos>NOLL)
              dias.diainfo[evnt.active].text_pos--;
            objc_edit(tree,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_INIT);
            return(TRUE);
            break;
          }
          case SC_RIGHT:
          {
            objc_edit(tree,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_END);
            if((dias.diainfo[evnt.active].text_pos<textlen)&&(dias.diainfo[evnt.active].text_pos<validlen))
              dias.diainfo[evnt.active].text_pos++;
            objc_edit(tree,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_INIT);
            return(TRUE);
            break;
          }
          default:
          {
            if(!(evnt.k_s&K_CTRL))
            {
              if(key)
              {
                objc_edit(tree,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_END);
                if((textlen<validlen)&&(tree[object].ob_flags&EDITABLE))
                {
                  if(((((TEDINFO *)tree[object].ob_spec)->te_pvalid[textlen]=='9')&&(isdigit(key)))
                  ||((((TEDINFO *)tree[object].ob_spec)->te_pvalid[textlen]=='a')&&(isalpha(key)||isspace(key)))
                  ||((((TEDINFO *)tree[object].ob_spec)->te_pvalid[textlen]=='n')&&(isalpha(key)||isdigit(key)||isspace(key)))
                  ||(((TEDINFO *)tree[object].ob_spec)->te_pvalid[textlen]=='X'))
                  {
                    ((TEDINFO *)tree[object].ob_spec)->te_ptext[textlen++]=key;
                    ((TEDINFO *)tree[object].ob_spec)->te_ptext[textlen]=NOLL;
                    objc_draw(tree,object,1,screenx,screeny,screenw,screenh);
                  }
                  else if(((((TEDINFO *)tree[object].ob_spec)->te_pvalid[textlen]=='A')&&(isalpha(key)||isspace(key)))
                  ||((((TEDINFO *)tree[object].ob_spec)->te_pvalid[textlen]=='N')&&(isalpha(key)||isdigit(key)||isspace(key)))
                  ||(((TEDINFO *)tree[object].ob_spec)->te_pvalid[textlen]=='x'))
                  {
                    key=toupper(key);
                    ((TEDINFO *)tree[object].ob_spec)->te_ptext[textlen++]=key;
                    ((TEDINFO *)tree[object].ob_spec)->te_ptext[textlen]=0;
                    objc_draw(tree,object,1,screenx,screeny,screenw,screenh);
                  }
                  else if(((TEDINFO *)tree[object].ob_spec)->te_pvalid[textlen]=='F')
                  {
                  }
                  else if(((TEDINFO *)tree[object].ob_spec)->te_pvalid[textlen]=='f')
                  {
                  }
                  else if(((TEDINFO *)tree[object].ob_spec)->te_pvalid[textlen]=='P')
                  {
                  }
                  else if(((TEDINFO *)tree[object].ob_spec)->te_pvalid[textlen]=='p')
                  {
                  }
                  else
                    return(FALSE);
                  dias.diainfo[evnt.active].text_pos=textlen;
                  objc_edit(tree,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_INIT);
                  return(TRUE);
                }
              }
            }
            else if(evnt.k_s&K_CTRL)
            {
              key=key_table[1][scan];
              switch(key)
              {
                case 'C':
                  tedcopy(tree,object);
                  return(TRUE);
                  break;
                case 'X':
                  objc_edit(tree,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_END);
                  tedcut(tree,object);
                  objc_edit(tree,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_INIT);
                  return(TRUE);
                  break;
                case 'V':
                  objc_edit(tree,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_END);
                  tedpaste(tree,object);
                  objc_edit(tree,dias.diainfo[evnt.active].text_object,0,&dias.diainfo[evnt.active].text_pos,ED_INIT);
                  return(TRUE);
                  break;
                default:
                  break;
              }
            }
            break;
          }
        }
        break;
      }
    }
  }
  return(FALSE);
}
/********************************************************************/
/********************************************************************/
void fix_message()
{
  int        c2,found=FALSE;
  WIN_DIALOG *info;
#ifdef WINTEXT
  WIN_TEXT   *text;
#endif

  for(c2=NOLL; c2<dias.number; c2++)
    if(evnt.data[3]==dias.window[c2]->ident)
    {
      info=dias.window[c2];
#ifdef WINTEXT
      if((info->type==TYPE_LIST)||(info->type==TYPE_TEXT))
        text=(WIN_TEXT*)info->text;
#endif
      found=TRUE;
    }
  tebax.data[SVAR_WINDOW_ID]=info->ident;
  tebax.data[SVAR_WINDOW_MESSAGE]=evnt.data[0];
  tebax.data[SVAR_WINDOW_DATA_0]=evnt.data[3];
  tebax.data[SVAR_WINDOW_DATA_1]=evnt.data[4];
  tebax.data[SVAR_WINDOW_DATA_2]=evnt.data[5];
  tebax.data[SVAR_WINDOW_DATA_3]=evnt.data[6];
  tebax.data[SVAR_WINDOW_DATA_4]=evnt.data[7];
  switch(evnt.data[0])
  {
    case MN_SELECTED:
    {
      tebax.type=MENU_CLICKED;
      tebax.data[SVAR_OBJECT]=evnt.data[4];
      tebax.data[SVAR_OBJECT_MENU_ROW]=evnt.data[3];
      evnt.end=TRUE;
      break;
    }
    case WM_ONTOP:
    case WM_UNTOPPED:
    case WM_NEWTOP:
      break;
    case WM_CLOSED:
      if(!((dias.nonmodal>0)&&(info->order!=dias.number)))
      {
        tebax.type=WINDOW_CLICKED;
        evnt.end=TRUE;
      }
      break;
    case WM_FULLED:
      if(!((dias.nonmodal>0)&&(info->order!=dias.number)))
      {
        if(info->type==TYPE_DIALOG)
        {
          tebax.type=WINDOW_CLICKED;
          evnt.end=TRUE;
        }
#ifdef WINTEXT
        else if((info->type==TYPE_LIST)||(info->type==TYPE_TEXT))
        {
          wind_get(info->ident,WF_CURRXYWH,&evnt.w1,&evnt.w2,&evnt.w3,&evnt.w4);
          if(evnt.w1==screenx && evnt.w2==screeny && evnt.w3==screenw && evnt.w4==screenh)
          {
            wind_get(info->ident,WF_PREVXYWH,&evnt.w1,&evnt.w2,&evnt.w3,&evnt.w4);
            wind_set(info->ident,WF_CURRXYWH,evnt.w1,evnt.w2,evnt.w3,evnt.w4);
          }
          else
            wind_set(info->ident,WF_CURRXYWH,screenx,screeny,screenw,screenh);
          wind_get(info->ident,WF_WORKXYWH,&(info->xy[X]),&(info->xy[Y]),&(info->xy[W]),&(info->xy[H]));
          if(text!=NULL)
          {
            if(text->dialog!=NULL)
            {
              ((OBJECT*)text->dialog)[ROOT].ob_x=info->xy[X];
              ((OBJECT*)text->dialog)[ROOT].ob_y=info->xy[Y];
            }
          }
        }
#endif
      }
      break;
    case WM_SIZED:
      if(!((dias.nonmodal>0)&&(info->order!=dias.number)))
      {
        if(info->type==TYPE_DIALOG)
        {
          tebax.type=WINDOW_CLICKED;
          evnt.end=TRUE;
        }
#ifdef WINTEXT
        else if((info->type==TYPE_LIST)||(info->type==TYPE_TEXT))
        {
          if(info->type==TYPE_TEXT)
          {
            if(xy[H]/info->text->char_h>info->text->num_of_rows)
              wind_set(info->ident,WF_VSLSIZE,1000,0,0,0);
            else
              wind_set(info->ident,WF_VSLSIZE,(SLIDESIZE*xy[H]/info->text->char_h)/info->text->num_of_rows,0,0,0);
          }
          else
          {
            wind_set(info->ident,WF_VSLSIZE,SLIDESIZE/info->text->num_of_rows,0,0,0);
          }
          wind_set(info->ident,WF_CURRXYWH,evnt.data[4],evnt.data[5],evnt.data[6],evnt.data[7]);
          wind_get(info->ident,WF_WORKXYWH,&(info->xy[X]),&(info->xy[Y]),&(info->xy[W]),&(info->xy[H]));
        }
#endif
      }
      break;
    case WM_ARROWED:
      if(!((dias.nonmodal>0)&&(info->order!=dias.number)))
      {
        if(info->type==TYPE_DIALOG)
        {
          tebax.type=WINDOW_CLICKED;
          evnt.end=TRUE;
        }
#ifdef WINTEXT
        else if((info->type==TYPE_LIST)||(info->type==TYPE_TEXT))
        {
          scroll_text(info,evnt.data[4]);
        }
#endif
      }
      break;
    case WM_HSLID:
      if(!((dias.nonmodal>0)&&(info->order!=dias.number)))
      {
        if(info->type==TYPE_DIALOG)
        {
          tebax.type=WINDOW_CLICKED;
          evnt.end=TRUE;
        }
#ifdef WINTEXT
        else if((info->type==TYPE_LIST)||(info->type==TYPE_TEXT))
        {
        }
#endif
      }
      break;
    case WM_VSLID:
      if(!((dias.nonmodal>0)&&(info->order!=dias.number)))
      {
        if(info->type==TYPE_DIALOG)
        {
          tebax.type=WINDOW_CLICKED;
          evnt.end=TRUE;
        }
#ifdef WINTEXT
        else if((info->type==TYPE_TEXT)||(info->type==TYPE_LIST))
        {
          xy[H]=info->xy[H];
          if(text->dialog!=NOLL)
            xy[H]-=text->dialog[ROOT].ob_height;
          if(info->type==TYPE_TEXT)
          {
            xy[Y]=evnt.data[4]*(text->num_of_rows-xy[H]/text->char_h)/1000;
            if(xy[Y]>text->num_of_rows)
              xy[Y]=text->num_of_rows;
            if(xy[Y]!=text->offset_y)
            {
              text->offset_y=xy[Y];
              update_text(info,NULL);
            }
          }
          else
          {
            xy[Y]=evnt.data[4]*(text->num_of_rows-1)/1000;
            if(xy[Y]!=text->select)
              set_mark(info,xy[Y],TRUE);
          }
        }
#endif
      }
      break;
    case WM_ICONIFY:
      if(found&&(info->icondata!=NULL))
        iconify_window(info);
      else
      {
        tebax.type=WINDOW_CLICKED;
        evnt.end=TRUE;
      }   
      break;
    case WM_UNICONIFY:
      if(info->type==TYPE_DIALOG)
      {
        if(found&&(info->dialog!=NULL))
          uniconify_window(info),evnt.found=TRUE;
        else
        {
          tebax.type=WINDOW_CLICKED;
          evnt.end=TRUE;
        } 
      }
#ifdef WINTEXT
      else if((info->type==TYPE_TEXT)||(info->type==TYPE_LIST))
        if(found&&(info->dialog!=NULL))
          uniconify_window(info),evnt.found=TRUE;
        else
        {
          tebax.type=WINDOW_CLICKED;
          evnt.end=TRUE;
        } 
#endif
      else
      {
        tebax.type=WINDOW_CLICKED;
        evnt.end=TRUE;
      }
      break;
    case WM_TOPPED:
      if(!((dias.nonmodal>0)&&(info->order!=dias.number)))
      {
        if(found)
          wind_set(info->ident,WF_TOP,info->ident,0,0,0), evnt.found=TRUE;
        else
        {
          tebax.type=WINDOW_CLICKED;
          evnt.end=TRUE;
        }
      }
      break;
    case WM_MOVED:
      if(found)
      {
        wind_set(evnt.data[3],WF_CURRXYWH,evnt.data[4],evnt.data[5],evnt.data[6],evnt.data[7]);
        if((info->status==WINDOW_ICONIZED)&&(info->icondata!=NULL))
        {
          wind_get(info->ident,WF_WORKXYWH,&xy[X],&xy[Y],&xy[W],&xy[H]);
          info->i_x=xy[X];
          info->i_y=xy[Y];
          info->icondata[ROOT].ob_x=xy[X];
          info->icondata[ROOT].ob_y=xy[Y];
        }
        else if(info->type==TYPE_DIALOG)
        {
          wind_get(info->ident,WF_WORKXYWH,&(info->xy[X]),&(info->xy[Y]),&(info->xy[W]),&(info->xy[H]));
          if(info->dialog!=NULL)
          {
            b_place(info->dialog,ROOT,xy);
            xy[X]-=((OBJECT*)info->dialog)[ROOT].ob_x;
            xy[Y]-=((OBJECT*)info->dialog)[ROOT].ob_y;
            ((OBJECT*)info->dialog)[ROOT].ob_x=info->xy[X]-xy[X];
            ((OBJECT*)info->dialog)[ROOT].ob_y=info->xy[Y]-xy[Y];
          }
        }
#ifdef WINTEXT
        else if((info->type==TYPE_LIST)||(info->type==TYPE_TEXT))
        {
          wind_get(info->ident,WF_WORKXYWH,&(info->xy[X]),&(info->xy[Y]),&(info->xy[W]),&(info->xy[H]));
          if(text!=NULL)
          {
            if(text->dialog!=NULL)
            {
              ((OBJECT*)text->dialog)[ROOT].ob_x=info->xy[X];
              ((OBJECT*)text->dialog)[ROOT].ob_y=info->xy[Y];
            }
          }
        }
#endif
        else
        {
          tebax.type=WINDOW_CLICKED;
          evnt.end=TRUE;
        }
      }
      else
      {
        tebax.type=WINDOW_CLICKED;
        evnt.end=TRUE;
      }
      break;
    case WM_REDRAW:
      if(found)
      {
        if((info->type==TYPE_DIALOG)&&(info->dialog!=NULL))
          update_dialog(info, evnt.data+4);
#ifdef WINTEXT
        else if(((info->type==TYPE_TEXT)||(info->type==TYPE_LIST))&&(info->text!=NULL))
          update_text(info, evnt.data+4);
#endif
      }
      else
      {
        tebax.type=WINDOW_CLICKED;
        evnt.end=TRUE;
      }
      break;
    case SH_WDRAW: /* what does this do? */
    case 0x5758:  /* Magic: "Show only Title-Bar" On  */
    case 0x5759:  /* Magic: "Show only Title-Bar" Off */
      break;
    case 0x4710:  /* AV_SENDKEY */
    {
      evnt.key=evnt.data[4];
      evnt.k_s=evnt.data[3];
      fix_key_clicked();
      break;
    }
    default:
      tebax.type=UNDEF_MESSAGE;
      memcpy(tebax.data,evnt.data,sizeof(short)*16);
      evnt.end=TRUE;
      break;
  }
}

/********************************************************************/
/* kollar om en short-key till menyn �r tryckt                      */
/********************************************************************/
int check_menu_shortkey(int key, int k_s)
{
  int counter=0,keys;
  while(shortkorr[counter]!=NOLL)
  {
    if(!(menutree[shortkorr[counter]].ob_state&DISABLED))
    {
      keys=NOLL;
      if(k_s&(K_RSHIFT|K_LSHIFT))
      {
        keys|=NU_SHIFT;
      }
      if(k_s&K_CTRL)
      {
        keys|=NU_CTRL;
      }
      if(k_s&K_ALT)
      {
        keys|=NU_ALT;
      }
      if(keys/256==shortkeys[counter]/256)
      {
        if((keys|NU_SHIFT)&&(toupper(key_table[1][key/256])==(shortkeys[counter]&255)))
        {
          return(shortkorr[counter]);
        }
        if((keys|NU_SHIFT)&&(toupper(key_table[0][key/256])==(shortkeys[counter]&255)))
        {
          return(shortkorr[counter]);
        }
      }
    }
    counter++;
  }
  return(FAIL);
}

/********************************************************************/
/* Changes the size of the ROOT object and the size of the window   */
/* To the size of the largest visible object (+ some extra space)   */
/********************************************************************/
void change_size(WIN_DIALOG *info)
{
  short x=((OBJECT *)info->dialog)[ROOT].ob_x,y=((OBJECT *)info->dialog)[ROOT].ob_y,w=NOLL,h=NOLL;
  int o_x,o_y,o_w,o_h;
  int object=ROOT,head,next,last=ROOT;
  
  do
  {
    head=((OBJECT *)info->dialog)[object].ob_head;
    next=((OBJECT *)info->dialog)[object].ob_next;
    o_x=((OBJECT *)info->dialog)[object].ob_x;
    o_y=((OBJECT *)info->dialog)[object].ob_y;
    o_w=((OBJECT *)info->dialog)[object].ob_width;
    o_h=((OBJECT *)info->dialog)[object].ob_height;
    if((object!=ROOT)&&(!(((OBJECT *)info->dialog)[object].ob_flags&HIDETREE)))
    {
      if(o_x+o_w>w)
        w=o_x+o_w;
      if(o_y+o_h>h)
        h=o_y+o_h;
    }
    if((head!=FAIL && last<=object)&&(!(((OBJECT *)info->dialog)[object].ob_flags&HIDETREE)))
        last=object,object=head;
    else
      last=object,object=next;
  }while(object>ROOT);
  h+=8,w+=8;
  ((OBJECT *)info->dialog)[ROOT].ob_width=w;
  ((OBJECT *)info->dialog)[ROOT].ob_height=h;
  if((info->status==WINDOW_OPENED)||(info->status==WINDOW_NMOPENED))
  {
    wind_calc(WC_BORDER, info->attr, ((OBJECT *)info->dialog)[ROOT].ob_x, ((OBJECT *)info->dialog)[ROOT].ob_y, ((OBJECT *)info->dialog)[ROOT].ob_width, ((OBJECT *)info->dialog)[ROOT].ob_height, &x, &y, &w, &h);
    wind_set(info->ident,WF_CURRXYWH,x,y,w,h);
    wind_get(info->ident,WF_WORKXYWH,&(info->xy[X]),&(info->xy[Y]),&(info->xy[W]),&(info->xy[H]));
  }
}
/********************************************************************/
/* Win_Dia Program Definitions (Non at the moment)                  */
/********************************************************************/
void draw_progdef(PARMBLK *parameter)
{
  
}



#ifdef WINLOGGING
/********************************************************************/
/* Skall fungera ungef�r som printf                                 */
/* %d - int                                                         */
/* %l - long                                                        */
/*  NOT %c - char                                                   */
/* %s - char *                                                      */
/********************************************************************/
#include <stdarg.h>
void WinLog(char *logstring,...)
{
  char *point=logstring;
  int num_parm=NOLL;
  va_list ap;
  FILE *logfile;

  logfile=fopen("dialog.log","a");
  if(logfile!=NOLL)
  {
    while(*point!=NOLL)
    {
      if(*point=='%')
      {
        point++;
        if((*point=='c')||(*point=='d')||(*point=='s')||(*point=='l'))
          num_parm++;
        if(*point==NOLL)
          point--;
      }
      point++;
    }
    if(num_parm>0)
      va_start(ap,logstring);
    point=logstring;
    
    while(*point!=NOLL)
    {
      if(*point=='%')
      {
        point++;
        if(*point=='%')
          fputc('%',logfile);
        else if(*point=='d')
          fprintf(logfile,"%d",va_arg(ap,int));
        else if(*point=='x')
          fprintf(logfile,"%x",va_arg(ap,int));
        else if(*point=='X')
          fprintf(logfile,"%lx",va_arg(ap,long));
        else if(*point=='l')
          fprintf(logfile,"%ld",va_arg(ap,long));
        else if(*point=='s')
          fprintf(logfile,"%s",va_arg(ap,char *));
        else if(*point!=NOLL)
        {
          fputc('%',logfile);
          fputc(*point,logfile);
        }
        else
          point--;
      }
      else
        fputc(*point,logfile);
      point++;
    }
    fputc('\n',logfile);
    fclose(logfile);
    va_end(ap);
  }
}
#endif

/********************************************************************/
/********************************************************************/
void tedcut(OBJECT *tree, int object)
{
  if(scrp_read(d_temp)==NOLL)
  {
    return;
  }
  strncat(d_temp,SCRAPFILE,MAXSTRING);
  d_fil=fopen(d_temp,"w");
  if(d_fil==NOLL)
    return ;
  fprintf(d_fil,"%s",((TEDINFO *)tree[object].ob_spec)->te_ptext);
  fclose(d_fil);
  ((TEDINFO *)tree[object].ob_spec)->te_ptext[NOLL]=NOLL;
  objc_draw(tree,object,1,screenx,screeny,screenw,screenh);
  dias.diainfo[evnt.active].text_pos=NOLL;
  dias.diainfo[evnt.active].text_object=object;
}

/********************************************************************/
/********************************************************************/
void tedcopy(OBJECT *tree, int object)
{
  if(scrp_read(d_temp)==NOLL)
  {
    return;
  }
  strncat(d_temp,SCRAPFILE,MAXSTRING);
  d_fil=fopen(d_temp,"w");
  if(d_fil==NOLL)
    return;
  fprintf(d_fil,"%s",((TEDINFO *)tree[object].ob_spec)->te_ptext);
    fclose(d_fil);
  dias.diainfo[evnt.active].text_object=object;
  if(dias.diainfo[evnt.active].text_pos>strlen(((TEDINFO *)tree[object].ob_spec)->te_ptext))
    dias.diainfo[evnt.active].text_pos=strlen(((TEDINFO *)tree[object].ob_spec)->te_ptext);

}
/********************************************************************/
/********************************************************************/
void tedpaste(OBJECT *tree,int object)
{
  if(scrp_read(d_temp)==NOLL)
  {
    return;
  }

  strncat(d_temp,SCRAPFILE,MAXSTRING);
  d_fil=fopen(d_temp,"r");
  if(d_fil==NOLL)
    return;
  fgets(d_temp,MAXSTRING-1,d_fil);
  fclose(d_fil);
  str2ted(tree,object,d_temp);
  objc_draw(tree,object,1,screenx,screeny,screenw,screenh);
  dias.diainfo[evnt.active].text_pos=strlen(d_temp);
  dias.diainfo[evnt.active].text_object=object;
}