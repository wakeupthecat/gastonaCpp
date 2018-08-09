/*
project gastonaCpp
Copyright (C) 2018 Alejandro Xalabarder Aulet

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#include "utilTree.h"

int main (int nn, char ** aa)
{
   EvaUnit modela = EvaUnit ("jala");

   Eva datos = Eva("poyos");
   Eva separ = Eva("poyos separator");
   Eva colu = Eva("poyos dataColumn");

   datos.addLine (EvaLine ("icon, node"));
   datos.addLine (EvaLine ("janso, rota&pesqui&narda"));
   datos.addLine (EvaLine ("coi  , rota&pesqui&comma"));
   datos.addLine (EvaLine ("coi  , rota&habba"));
   datos.addLine (EvaLine ("coi  , rota&hezarifen&celebi"));
   datos.addLine (EvaLine ("coi  , rota&hezarifen&ahalad"));
   datos.addLine (EvaLine ("coi  , barna&ensanche&clinic"));
   datos.addLine (EvaLine ("coi  , barna&ensanche&caspe"));
   datos.addLine (EvaLine ("coi  , barna&horta&clot"));

   separ.addLine (EvaLine ("&"));
   colu.addLine (EvaLine ("node"));

   modela.add (datos);
   modela.add (separ);
   modela.add (colu);

   EBSTree etra = EBSTree ("poyos", modela);

   eva2TreeStruct asserrao = utilTree::sierra (etra);

   printf ("%s", asserrao.eva.toString ().c_str ());
}

//
//   <TIS(tree intern structure)>
//      janso,rota,pesqui,narda
//      coi  ,    ,      ,comma
//      coi  ,    ,habba
//      coi  ,    ,hezarifen,celebi
//      coi  ,    ,         ,ahalad
//      coi  ,barna,ensanche,clinic
//      coi  ,     ,        ,caspe
//      coi  ,     ,horta   ,clot