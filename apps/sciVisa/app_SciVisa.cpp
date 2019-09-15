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
class appSciVisaController : public MensakaTarget
{
public:
   enum
   {
      MSK_FRIST_DATA_LOAD,
      MSK_LISTA_SELECCIONADA,
   };

   appSciVisaController ()
   {
      Mensaka::subscribeToMessage (gastonaCGlobals::msgDATA_FIRST_LOAD (), MSK_FRIST_DATA_LOAD, this);
      Mensaka::subscribeToMessage ("iFiles",  MSK_LISTA_SELECCIONADA, this);
   }

   ~appSciVisaController ()
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
               // FillListBox (pk);
            }
            break;
         // case MSK_BOTON_PULSADO:
         // case MSK_EDIT_ENTRADO:
         //    {
         //       string val = pk["ePasoto"].copyValue ();
         //       vector<string> pars;
         //       pars.push_back ("");
         //       pars.push_back ((mappedMsg == MSK_BOTON_PULSADO ? "Del boton: [" : "Del edit: [") + val + "]");
         //       Mensaka::sendPacket ("lLabello data!", pars);
         //    }
         //    break;
         //case MSK_TABLA_SELECCIONADA:
         //   {
         //      string val = pk["tAmbrosia selected.name"].copyValue ();
         //      vector<string> pars;
         //      pars.push_back ("");
         //      pars.push_back ("De la tabla: [" + val + "]");
         //      Mensaka::sendPacket ("lLabello data!", pars);
         //   }
         //   break;
         case MSK_LISTA_SELECCIONADA:
            {
               string val = pk["iFiles selected.name"].copyValue ();
               TRACE (("selected value [%s]\n", val.c_str ()));

               pk.ensureEva ("zScintillo fileName");
               // pk["zScintillo fileName"][0][0] = val;
               pk.setValue ("zScintillo fileName", val.c_str ());
               Mensaka::sendPacket ("zScintillo data!");
            }
            break;
         default:
            return false;
      }
      return true;
   }
};

// quasi Singelton instance of LogicController, since it is connected through mensaka nothing else is needed
//
static appSciVisaController conto;
