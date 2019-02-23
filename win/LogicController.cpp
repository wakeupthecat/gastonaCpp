/*
Gastona C++
Copyright (C) 2016-2019  Alejandro Xalabarder Aulet

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

#include "GastonaWin.h"

using namespace std;

// the controller will be instanciated at the end as a singleton

/**
   en este caso la lógica
      - carga la lista con las variables de entorno
      - regoge la seleccion de la lista, busca la variable de entorno y la envia al label

   puro listix
*/
class LogicController : public MensakaTarget
{
public:
   enum
   {
      MSK_FRIST_DATA_LOAD,
      MSK_LISTA_SELECCIONADA,
      MSK_TABLA_SELECCIONADA,
      MSK_EDIT_ENTRADO,
      MSK_BOTON_PULSADO,
   };

   LogicController ()
   {
      Mensaka::subscribeToMessage (gastonaCGlobals::msgDATA_FIRST_LOAD (), MSK_FRIST_DATA_LOAD, this);
      Mensaka::subscribeToMessage ("tAmbrosia", MSK_TABLA_SELECCIONADA, this);
      Mensaka::subscribeToMessage ("iRogelia",  MSK_LISTA_SELECCIONADA, this);
      Mensaka::subscribeToMessage ("ePasoto",   MSK_EDIT_ENTRADO, this);
      Mensaka::subscribeToMessage ("bSabroson", MSK_BOTON_PULSADO, this);
   }

   ~LogicController ()
   {
   }


   bool takePacket (int mappedMsg, EvaUnit & pk, const vector<string> & params)
   {
      int iLength = 0;

      TCHAR * pVarName = 0;
      TCHAR * pVarValue = 0;

      switch (mappedMsg)
      {
         case MSK_FRIST_DATA_LOAD:
            {
               FillListBox (pk);
            }
            break;
         case MSK_BOTON_PULSADO:
         case MSK_EDIT_ENTRADO:
            {
               string val = pk["ePasoto"].copyValue ();
               vector<string> pars;
               pars.push_back ("");
               pars.push_back ((mappedMsg == MSK_BOTON_PULSADO ? "Del boton: [" : "Del edit: [") + val + "]");
               Mensaka::sendPacket ("lLabello data!", pars);
            }
            break;
         case MSK_TABLA_SELECCIONADA:
            {
               string val = pk["tAmbrosia selected.name"].copyValue ();
               vector<string> pars;
               pars.push_back ("");
               pars.push_back ("De la tabla: [" + val + "]");
               Mensaka::sendPacket ("lLabello data!", pars);
            }
            break;
         case MSK_LISTA_SELECCIONADA:
            {
               string val = pk["iRogelia selected.varname"].copyValue ();
               pVarName = (TCHAR *) val.c_str ();

               printf ("selected value [%s]\n", val.c_str ());

               // Get environment string.
               iLength = GetEnvironmentVariable (pVarName, NULL, 0);
               pVarValue = (TCHAR *) calloc (iLength + 1, sizeof (TCHAR));

               GetEnvironmentVariable (pVarName, pVarValue, iLength);
               pVarValue[iLength] = 0;

               vector<string> pars;
               pars.push_back ("");
               pars.push_back (pVarValue);

               Mensaka::sendPacket ("lLabello data!", pars);
               free (pVarValue);
            }
            break;
         default:
            return false;
      }
      return true;
   }

   void FillListBox (EvaUnit & euData)
   {
      int     iLength;
      TCHAR * pVarBlock, * pVarBeg, * pVarEnd, * pVarName;

      pVarBlock = GetEnvironmentStrings ();  // Get pointer to environment block

      Eva & eva = euData.getSomeHowEva ("iRogelia");
      eva.clearLines ();
      eva.addLine (EvaLine ("varname, value"));

      while (*pVarBlock)
      {
         if (*pVarBlock != '=')
         {
            pVarBeg = pVarBlock;              // Beginning of variable name
            while (*pVarBlock++ != '=');      // Scan until '='
            pVarEnd = pVarBlock - 1;          // Points to '=' sign
            iLength = pVarEnd - pVarBeg;      // Length of variable name


            // Allocate memory for the variable name and terminating
            // zero. Copy the variable name and append a zero.

            pVarName = (TCHAR *) calloc (iLength + 1, sizeof (TCHAR));
            if (pVarName)
            {
               CopyMemory (pVarName, pVarBeg, iLength * sizeof (TCHAR));
               pVarName[iLength] = 0;
            }

            // Put the variable name in the list box and free memory.
            eva.addLine (EvaLine (string ((char *)pVarName) + ", value?"));
            free (pVarName);
         }
         while (*pVarBlock++ != '\0');     // Scan until terminating zero
      }
      FreeEnvironmentStrings (pVarBlock);
   }
};

// quasi Singelton instance of LogicController, since it is connected through mensaka nothing else is needed
//
static LogicController conto;
