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

#ifndef MENSAKA_HEADER_H
#define MENSAKA_HEADER_H


#include <map>

#include "EvaLine.h"
#include "Eva.h"
#include "EvaUnit.h"

using namespace std;

class MensakaTarget
{
   public:

   virtual bool takePacket (int mappedMsg, EvaUnit & pk, const vector<string> & params) = 0;
};


class Mensaka
{
public:

   struct subscriptor
   {
      subscriptor (MensakaTarget *ptr, int mapInt):
         ptrSubscriptor (ptr),
         msgMapInt (mapInt)
      {}

      MensakaTarget * ptrSubscriptor;
      int             msgMapInt;
   };

   typedef vector<subscriptor> subscriberVec;
   typedef map<string, subscriberVec>::iterator autoMsg;

   static map<string, subscriberVec> & getMessageMap ()
   {
      return PII_getMessageMap ();
   }

   static int sendPacket (const string & message,
                          const vector<string> & params = vector<string>())
   {
      return PII_sendPacket (message, params);
   }

   static int sendPacket (const string & message,
                          EvaUnit & pk,
                          const vector<string> & params = vector<string>())
   {
      return PII_sendPacket (message, pk, params);
   }

   //static void declareMessage (const string & message);

   static void subscribeToMessage (const string & message, int mappId, MensakaTarget * targetPtr)
   {
      PII_subscribeToMessage (message, mappId, targetPtr);
   }


protected:
   // PII = protected inline implementation
   // section for inline methods of more than one line
   // since the methods are protected they are not exposed to compilation
   // on each include of the header


   static map<string, subscriberVec> & PII_getMessageMap ()
   {
      static map<string, subscriberVec> messageMapObj;
      return messageMapObj;
   }

   static int PII_sendPacket (const string & message, const vector<string> & params = vector<string>())
   {
      EvaUnit eu;

      return sendPacket (message, eu, params);
   }

   static int PII_sendPacket (const string & message,
                             EvaUnit & pk,
                             const vector<string> & params = vector<string>())
   {
      int posi = 0;
      autoMsg ite = getMessageMap ().find (message);
      if (ite != getMessageMap ().end ())
      {
         subscriberVec & vec = ite -> second;
         for (int ss = 0; ss < vec.size (); ss ++)
         {
            if (vec[ss].ptrSubscriptor -> takePacket (vec[ss].msgMapInt, pk, params))
               posi ++;
         }
      }
      else
      {
         // message lost!
      }
      return posi;
   }

   //static void PII_declareMessage (const string & message);

   static void PII_subscribeToMessage (const string & message, int mappId, MensakaTarget * targetPtr)
   {
      subscriptor su = subscriptor (targetPtr, mappId);

      autoMsg ite = getMessageMap ().find (message);
      if (ite == getMessageMap ().end ())
      {
         // message unkown, first subscriptor!
         subscriberVec vec;
         vec.push_back (su);

         getMessageMap () [message] = vec;
      }
      else
      {
         subscriberVec & vec = ite -> second;
         vec.push_back (su);
      }
   }

};

#endif  // _HEADER_H


//   /**
//      Sends a packet (a message with info) to subscribers (if any)
//      Everybody (every method in every class) can send such a packet, even no subscribers for this
//      message are needed. In case there are no subscribers for this message at that moment then
//      the message is lost (not obviously!) and the function returns 0.
//
//      @return the number of subscribers for this message THAT HAVE RETURN true in its method takePacket
//
//      Example:
//         Mensaka.sendPacket ("hello world packettery", null);
//   */
//   static int sendPacket (const string & message, EvaUnit & pk, vector<string> & params)
//
//   /**
//      Subscribes an object of a class (that implements MensakaTarget) to receive a specific
//      message 'msgId'
//
//      @param msgID the message that 'theTarget' wants to receive up to this moment
//      @param mappID a internal (to 'theTarget') identifier of type int that will identifies the message 'msgID'
//      @param theTarget the class that implements MensakaTarget and want to receive the message 'msgID'
//
//      Example:
//
//         .. body of a class X that implements MensakaTarget
//
//         int MSG_BYE_BYE = 55;
//
//         Mensaka.subscribe ("adios", MSG_BYE_BYE, this);
//
//         // implementation od MensakaTarget
//         public boolean takePacket (int mappedMsg, EvaUnit pk) {
//            switch (mappedMsg) {
//               case MSG_BYE_BYE:
//                  System.out.println ("chiao");
//                  return true;
//               default: break;
//            }
//            return false;
//         }
//
//   */
//   static void subscribe (const string & msgId, int mappId, MensakaTarget * theTarget)
//
