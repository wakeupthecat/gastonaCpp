/*
Eva library C++ (www.elxala.de)
Copyright (c) 2006,2014 Alejandro Xalabarder Aulet

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


/*
   EvaLayoutManager.cpp

   author Alejandro Xalabarder
   date   31.05.2014


*/

#include <stdio.h>
#include <stdlib.h>

#include "EvaLayoutManager.h"
#include "traceMacro.h"

EvaLayoutManager::EvaLayoutManager ():
   changeInWidgets (false),
   showingStack (),
   currentShownLayout (),
   euLayoutInfo (),
   arrLayWidgets (),
   arrPtrLayouts ()
{
  gettimeofday (&tvSTART, NULL);
  TRACE (("EvaLayoutManager debug messages on"));
}

void EvaLayoutManager::setLayouts (EvaUnit & allLayouts)
{
   euLayoutInfo = allLayouts;
   for (int ii = 0; ii < arrPtrLayouts.size (); ii ++)
      if (arrPtrLayouts[ii])
         free (arrPtrLayouts[ii]);

   arrPtrLayouts.clear ();
   for (int ii = 0; ii < euLayoutInfo.size (); ii ++)
   {
      string PREF = "layout of ";
      string evaname = euLayoutInfo[ii].getName ();
      int carcas = evaname.find (PREF);
      if (evaname.find (PREF) == 0)
      {
         string layname = evaname.substr (PREF.length ());
         TRACE (("add layout <%s>", layname.c_str ()));
         arrPtrLayouts.push_back (new EvaLayout (self (), layname));
      }
   }
}

void EvaLayoutManager::addWidget (const zWidgetLayable & winWid)
{
   arrLayWidgets.push_back (winWid);
   changeInWidgets = true;
   currentShownLayout = "";
}


void EvaLayoutManager::maskElement (const string & elToMask, const string & maskElement)
{
   masks[elToMask] = maskElement;
   changeInWidgets = true;
}

string EvaLayoutManager::doYouMean (const string & namewanted)
{
   string nameMask = namewanted;
   uniqueTextStack circularCtrl;

   do
   {
      autoMap it = masks.find (nameMask);
      if (it == masks.end ()) break;

      nameMask = it->second;
      if (nameMask.length () == 0)
      {
         // it is unmasked explicitly (mask = "")
         return namewanted;
      }

      if (!circularCtrl.checkPushable (nameMask))
      {
         TRACERROR (("masks for %s found circular!", namewanted.c_str ()));
         return namewanted;
      }
      circularCtrl.push (nameMask);
   } while (true);

   return nameMask;
}



//
//    EvaLayoutManager
//

void EvaLayoutManager::doShowByNameAt (const string & oname, int x0, int y0, int dx, int dy)
{
   string rename = doYouMean (oname);

   int indx = getWidgetIndex (rename);
   if (indx >= 0)
   {
      //printf ("doShowByNameAt widget %s %d %d %d x %d!!", rename.c_str (), x0, y0, dx, dy);
      arrLayWidgets[indx].doShowAt (x0, y0, dx, dy);
      return;
   }
   indx = getLayoutIndex (rename);
   if (indx >= 0 && arrPtrLayouts[indx] != 0)
   {
      //printf ("doShowByNameAt layout %s %d %d %d x %d!!", rename.c_str (), x0, y0, dx, dy);

      //check no endless recursion
      if (! showingStack.checkPushable (rename))
      {
         TRACERROR (("doShowByNameAt error recursive show %s!!", rename.c_str ()));
         return;
      }

      // push name
      showingStack.push (rename);

      arrPtrLayouts[indx]->doShowAt (x0, y0, dx, dy);

      // pop name
      //showingStack.pop (rename);
      return;
   }

   TRACERROR (("doShowByNameAt don't know what to show!! %s %d %d %d x %d!!", rename.c_str (), x0, y0, dx, dy));
}

// main method doShowLayout
//     it is not called recursively by EvaLayoutManager
void EvaLayoutManager::doShowLayout (const string & cname, int dx, int dy)
{
   TRACE (("doShowLayout '%s' size %d x %d", cname.c_str (), dx, dy));
   // refresh and recalculation controls
   //
   if (changeInWidgets || currentShownLayout != cname)
   {
      // widgets has been changed, invalidate all precalculations
      // in layouts
      for (int ll = 0; ll < arrPtrLayouts.size (); ll ++)
         if (arrPtrLayouts[ll] != 0)
            arrPtrLayouts[ll]->invalidate ();

      for (int ii = 0; ii < arrLayWidgets.size (); ii ++)
      {
         TRACE1 (("hide %d '%s'", ii, arrLayWidgets[ii].layableData.name.c_str ()));
         arrLayWidgets[ii].doHide ();
      }
      currentShownLayout = cname;
   }
   changeInWidgets = false;

   showingStack.clear ();
   // call EvaLayout
   //
   // TRACE (("DBG: start series 1000"));
   // for (int zz = 0; zz < 1000; zz ++)
   doShowByNameAt (cname, 0, 0, dx, dy);
   // TRACE (("DBG: finish series 1000"));
   showingStack.clear ();
}

int EvaLayoutManager::getWidgetIndex (const string & oname)
{
   for (int ii = 0; ii < arrLayWidgets.size (); ii ++)
      if (arrLayWidgets[ii].getName () == oname)
         return ii;
   return -1;
}

int EvaLayoutManager::getLayoutIndex (const string & oname)
{
   for (int ii = 0; ii < arrPtrLayouts.size (); ii ++)
      if (arrPtrLayouts[ii] != 0 && arrPtrLayouts[ii]->getName () == oname)
         return ii;
   return -1;
}

EvaLayable & EvaLayoutManager::getLayableByName (const string & oname, layableType & type)
{
   string rename = doYouMean (oname);

   type = IS_WIDGET;
   int indx = getWidgetIndex (rename);
   if (indx >= 0)
      return arrLayWidgets[indx].layableData;

   type = IS_EVALAYOUT;
   indx = getLayoutIndex (rename);
   if (indx >= 0 && arrPtrLayouts[indx] != 0)
   {
      // Note : getLayableByName is called in doShowAt
      //        so is a candidate for endless recursion
      //        here we just prevent calling precalculateAll
      if (showingStack.checkPushable (rename))
         arrPtrLayouts[indx] -> precalculateAll ();
      return arrPtrLayouts[indx] -> layableData;
   }

   type = IS_OTHER;
   static EvaLayable dummy (":;?dummy");
   return dummy;
}


//
//    EvaLayoutManager::EvaLayout
//

EvaLayoutManager::EvaLayout::renglon::renglon (const string & rtype):
   type(FIXLEN),
   length(0),
   extraProp(0.f)
{
   if (rtype.length () == 0 || rtype[0] == HEADER_ADAPT)
   {
      type = MAXMIN;
   }
   else if (rtype[0] == HEADER_EXPAND)
   {
      type = EXPAND;
      // afterwards calculate the percentage
      extraProp = (rtype.length () < 2) ? 1.f: (float) atof (rtype.substr (1).c_str ());
   }
   else
   {
      type = FIXLEN; // explicit size
      length = atoi(rtype.c_str());
   }
}


void EvaLayoutManager::EvaLayout::precalculateAll ()
{
   if (isPrecalculated) return;

   TRACE1 (("precalculateAll () '%s'", layableData.name.c_str ()));
   Hmargin = max(0, atoi (layoutInfo[0][1].c_str()));
   Vmargin = max(0, atoi (layoutInfo[0][2].c_str()));
   Hgap    = max(0, atoi (layoutInfo[0][3].c_str()));
   Vgap    = max(0, atoi (layoutInfo[0][4].c_str()));
   Hdims.clear ();
   Vdims.clear ();

   // finding all components in the layout array

   float totExtraV = 0.f;
   float totExtraH = 0.f;

   // NOTE! number of columns might change for each row!
   // at the end of the for Hdims.size() is the max nCol of the evaInfo)
   int nCols = 0;
   int nRows = layoutInfo.rows()-2;

   for (int rr = 0; rr < nRows; rr ++)
   {
      Vdims.push_back (renglon (rowHeader (rr)));
      totExtraV += Vdims[rr].extraProp;
      nCols = layoutInfo.cols (2+rr)-1;

      for (int cc = 0; cc < nCols; cc ++)
      {
         // add only when the column is new (once per column!)
         if (cc >= Hdims.size ())
         {
            Hdims.push_back (renglon (columnHeader (cc)));
            totExtraH += Hdims[cc].extraProp;
         }

         string & wname = widgetAt(rr, cc);
         layableType typ;
         EvaLayable & layData = layManager.getLayableByName (wname, typ);

         if (typ == IS_OTHER) continue;

         // either IS_WIDGET or IS_EVALAYOUT

         //printf ("precalculateAll componente %s", wname.c_str ());

         // set position x,y
         layData.cellPos.x = cc;
         layData.cellPos.y = rr;

         // set position x2,y2
         // for that check if next possible horizontal and vertical expansions (-, --, +, ++)
         //
         int ava = cc;
         while (ava+1 < nCols && expansionAt(rr, ava+1) == EXPAND_HORIZONTAL) ava ++;
         if (ava+1 < nCols && expansionAt(rr, ava+1) == EXPAND_HORIZONTAL_TOEND) ava = -1;
         layData.cellPos.dx = ava;
         if (ava == cc)
         {
            // add contribution of the cell rr, cc to the MAXMIN computation of rows
            // only if this widget does not expand horinzontally!
            //
            TRACE1 (("add contribution of '%s' to minmax of column %d", wname.c_str (), cc));
            if (Hdims[cc].type == MAXMIN)
               Hdims[cc].length = max(Hdims[cc].length, layData.iniRect.dx);
         }

         ava = rr;
         while (ava+1 < nRows && expansionAt(ava+1, cc) == EXPAND_VERTICAL) ava ++;
         if (ava+1 < nRows && expansionAt(ava+1, cc) == EXPAND_VERTICAL_TOEND) ava = -1;
         layData.cellPos.dy = ava;
         if (ava == rr)
         {
            // add contribution of the cell rr, cc to the MAXMIN computation of columns
            // only if this widget does not expand vertically!
            //
           TRACE1 (("add contribution of '%s' to minmax of row %d", wname.c_str (), rr));
           if (Vdims[rr].type == MAXMIN)
              Vdims[rr].length = max(Vdims[rr].length, layData.iniRect.dy);
         }

        TRACE1 (("precalculateAll componente %s is laid out true!!", wname.c_str ()));
        layData.isLaidOut = true;
     }
   }

   fijoV = Vmargin + Vmargin + Vgap * (Vdims.size () - 1);
   for (int rr = 0; rr < Vdims.size (); rr ++)
   {
      fijoV += Vdims[rr].length;
      TRACE1 (("VextraProp[%d] %.2f total %.2f", rr, Vdims[rr].extraProp, totExtraV ));
      Vdims[rr].extraProp = totExtraV > 0.f ? (Vdims[rr].extraProp / totExtraV): 0;
      //TRACE1 (("BextraProp[%d] %.2f", Vdims[rr].extraProp, totExtraV ));
   }

   fijoH = Hmargin + Hmargin + Hgap * (Hdims.size () - 1);
   for (int cc = 0; cc < Hdims.size (); cc ++)
   {
      fijoH += Hdims[cc].length;
      TRACE1 (("HextraProp[%d] %.2f total %.2f", cc, Hdims[cc].extraProp, totExtraH ));
      Hdims[cc].extraProp = totExtraH > 0.f ? (Hdims[cc].extraProp / totExtraH): 0;
   }
   layableData.iniRect.y  = 0;
   layableData.iniRect.dy = fijoV;
   layableData.iniRect.x  = 0;
   layableData.iniRect.dx = fijoH;

   TRACE1 (("precalculateAll () finished, fijoV %d, fijoH %d", fijoV, fijoH));
   isPrecalculated = true;
}

void EvaLayoutManager::EvaLayout::doShowAt(int x0, int y0, int totWidth, int totHeight)
{
   precalculateAll ();

   TRACE1 (("%s doShowAt (%d %d %d %d)", getName ().c_str (), x0, y0, totWidth, totHeight));

   float totalExtraV = (totHeight - fijoV);
   float totalExtraH = (totWidth - fijoH);

   int posH = Hmargin;
   for (int cc = 0; cc < Hdims.size (); cc ++)
   {
      if (cc > 0)
      {
         // add the size of the previous column
         posH += Hgap + Hdims[cc-1].length + Hdims[cc-1].extraProp * totalExtraH;
      }
      int posV = Vmargin;
      for (int rr = 0; rr < Vdims.size (); rr ++)
      {
         if (rr > 0)
         {
            // add the size of the previous row
            posV += Vgap + Vdims[rr-1].length + Vdims[rr-1].extraProp * totalExtraV;
         }

         string & wname = widgetAt(rr, cc);
         layableType typ;
         EvaLayable & wi = layManager.getLayableByName (wname, typ);

         if (typ == IS_OTHER) continue;

         if (wi.isLaidOut)
         {
            int x = posH;
            int y = posV;
            int dx = -Hgap;
            int dy = -Vgap;
            int pp = wi.cellPos.x;
            int pp2 = wi.cellPos.dx == -1 ? Hdims.size()-1: wi.cellPos.dx;
            for (; pp <= pp2 ; pp ++)
            {
                if (pp < Hdims.size())
                   dx += Hgap + Hdims[pp].length + (int)(Hdims[pp].extraProp * totalExtraH);
                else TRACERROR(("inconsistent layout horizontal information for widget %s, col %d but num of cols is %l", getName().c_str(), pp, Hdims.size()));
            }
            pp = wi.cellPos.y;
            pp2 = wi.cellPos.dy == -1 ? Vdims.size()-1: wi.cellPos.dy;
            //for (; pp <= pp2 && pp < Vdims.size (); pp ++)
            for (; pp <= pp2 ; pp ++)
            {
                if (pp < Vdims.size())
                    dy += Vgap + Vdims[pp].length + (int)(Vdims[pp].extraProp * totalExtraV);
                else TRACERROR(("inconsistent layout vertical information for widget %s, row %d but num of rows is %l", getName().c_str(), pp, Vdims.size()));
            }

            if (x0+x < 0 || y0 + y < 0 || dx < 0 || dy < 0) continue;

            TRACE1 (("doShowByNameAt ('%s', %d, %d, %d, %d)", wi.name.c_str (), x0 + x, y0 + y, dx, dy));
            layManager.doShowByNameAt (wi.name, x0 + x, y0 + y, dx, dy);
         }
         else
         {
            TRACE1 (("doHideByName ('%s')", wi.name.c_str ()));
            layManager.doHideByName (wi.name);
         }
      }
   }
}

string & EvaLayoutManager::EvaLayout::columnHeader(int ncol)
{
   return layoutInfo[1][ncol + 1];
}

string & EvaLayoutManager::EvaLayout::rowHeader(int nrow)
{
   return layoutInfo[2 + nrow][0];
}
