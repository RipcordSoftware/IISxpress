// TestEventHandlePool.h

#pragma once

using namespace System;
using namespace NUnit::Framework;

#include "HandlePool.h"

#include <set>

namespace TestEventHandlePool 
{
	[TestFixture]
	public ref class TestHandleObject
	{

	public:

		[Test]
		void EmptyHandleObjectIsNotValid() 
		{
			HandleObject handleObject;
			Assert::IsFalse(handleObject.IsValid(), "Empty handle objects should be invalid");
		}

		[Test]
		void HandleObjectIsValidAndCloses() 
		{
			HANDLE hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

			{
				HandleObject handleObject(hEvent);
				Assert::IsTrue(handleObject.IsValid(), "Populated handle objects should be valid");
			}

			Assert::IsFalse(::CloseHandle(hEvent) == TRUE, "The handle object did not free the event handle");
		}

		[Test]
		void HandleObjectIsDetachedAndDoesntClose() 
		{
			HANDLE hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
			
			{
				HandleObject handleObject(hEvent);
				HANDLE hTemp = handleObject.Detach();

				Assert::AreEqual((unsigned) hEvent, (unsigned) hTemp, 
					"The Detach()ed handle did not match the constructed handle");

				Assert::IsFalse(handleObject.IsValid(), "The Detach()ed handle object should be empty");
			}

			Assert::IsTrue(::CloseHandle(hEvent) == TRUE, "The handle object should not free the event handle");
		}

		[Test]
		void HandleObjectIsCorrect() 
		{
			HANDLE hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
			
			HandleObject handleObject(hEvent);
			HANDLE hTemp = handleObject;

			Assert::AreEqual((unsigned) hEvent, (unsigned) hTemp, 
				"The handle did not match the constructed handle");
		}

		[Test]
		void HandleObjectIsCorrectAndDoesntDetach() 
		{
			HANDLE hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
			
			{
				HandleObject handleObject(hEvent);
				HANDLE hTemp = handleObject;

				Assert::AreEqual((unsigned) hEvent, (unsigned) hTemp, 
					"The handle did not match the constructed handle");

				Assert::IsTrue(handleObject.IsValid(), "The handle object should not be empty");
			}			

			Assert::IsFalse(::CloseHandle(hEvent) == TRUE, "The handle object should free the event handle");
		}

		[Test]
		void HandleObjectSlotIsInitialisedToDefault1() 
		{			
			HandleObject handleObject;
			Assert::IsTrue(handleObject.GetSlot() == -1, "The default slot is not -1");
		}

		[Test]
		void HandleObjectSlotIsInitialisedToDefault2() 
		{
			HANDLE hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

			HandleObject handleObject(hEvent);
			Assert::IsTrue(handleObject.GetSlot() == -1, "The default slot is not -1");
		}

		[Test]
		void HandleObjectSlotIsInitialised() 
		{
			HANDLE hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

			HandleObject handleObject(hEvent, 99);
			Assert::IsTrue(handleObject.GetSlot() == 99, "The initialised slot is not 99");
		}

		[Test]
		void HandleObjectCookieIsAssigned() 
		{			
			HandleObject handleObject;
			handleObject.SetCookie(99);
			Assert::IsTrue(handleObject.GetCookie() == 99, "The initialised slot is not 99");
		}

		[Test]
		void HandleObjectIsInitialisedAndCookieIsAssigned() 
		{			
			HANDLE hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

			HandleObject handleObject(hEvent);
			handleObject.SetCookie(99);

			Assert::IsTrue(handleObject.GetCookie() == 99, "The initialised slot is not 99");
		}

		[Test]
		void EmptyHandleObjectIsCopyConstructedByEmpty()
		{
			HandleObject handleObject1;			

			HandleObject handleObject2(handleObject1);
			Assert::IsFalse(handleObject2.IsValid(), "The empty copy constructed handle object is not empty");
		}

		[Test]
		void EmptyHandleObjectIsCopyConstructedByPopulated()
		{
			HANDLE hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

			{
				const unsigned nSlot = 99;
				const unsigned nCookie = 1234;
				HandleObject handleObject1(hEvent, nSlot);
				handleObject1.SetCookie(nCookie);

				HandleObject handleObject2(handleObject1);

				Assert::IsFalse(handleObject1.IsValid(), "The copy construction donor handle object is not empty");
				Assert::IsTrue(handleObject2.IsValid(), "The copy constructed handle object is empty");

				Assert::IsTrue(handleObject2.GetSlot() == nSlot, "The copy constucted object has an invalid slot");
				Assert::IsTrue(handleObject2.GetCookie() == nCookie, "The copy constucted object has an invalid cookie");
			}

			Assert::IsFalse(::CloseHandle(hEvent) == TRUE, "The handle object should free the event handle");
		}

		[Test]
		void EmptyHandleObjectIsAssignedByEmpty()
		{
			HandleObject handleObject1;			

			HandleObject handleObject2;
			handleObject2 = handleObject1;

			Assert::IsFalse(handleObject1.IsValid(), "The object is not empty");
			Assert::IsFalse(handleObject2.IsValid(), "The object is not empty");
		}

		[Test]
		void EmptyHandleObjectIsAssignedByPopulated()
		{
			HANDLE hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
			
			{
				const unsigned nSlot = 99;
				const unsigned nCookie = 1234;
				HandleObject handleObject1(hEvent, nSlot);
				handleObject1.SetCookie(nCookie);

				HandleObject handleObject2;			
				handleObject2 = handleObject1;

				Assert::IsFalse(handleObject1.IsValid(), "The assigned donor handle object is not empty");
				Assert::IsTrue(handleObject2.IsValid(), "The assigned handle object is empty");

				Assert::IsTrue(handleObject2.GetSlot() == nSlot, "The assigned object has an invalid slot");
				Assert::IsTrue(handleObject2.GetCookie() == nCookie, "The copy constucted object has an invalid cookie");
			}

			Assert::IsFalse(::CloseHandle(hEvent) == TRUE, "The handle object should free the event handle");
		}
	};

	[TestFixture]
	public ref class TestEventHandlePool
	{

	public:

		[Test]
		void TestConstruct()
		{
			unsigned nSize = 16;
			EventHandlePool pool(nSize);
			Assert::AreEqual((unsigned) pool.GetSize(), nSize, "The pool size does not match the constructor");
		}

		[Test]
		void TestEmptyReturn()
		{			
			EventHandlePool pool;

			HandleObject handleObject;
			Assert::IsFalse(pool.ReturnHandleObject(handleObject), "ReturnHandleObject() has returned true");
		}

		[Test]
		void TestUniqueCookie()
		{			
			EventHandlePool pool1;
			EventHandlePool pool2;

			HandleObject handleObject1;
			Assert::IsTrue(pool1.GetHandleObject(handleObject1), "GetHandleObject() has returned false");

			HandleObject handleObject2;
			Assert::IsTrue(pool2.GetHandleObject(handleObject2), "GetHandleObject() has returned false");

			Assert::AreNotEqual((unsigned) handleObject1.GetCookie(), (unsigned) handleObject2.GetCookie(), 
				"The pool objects have the same cookie");
		}

		[Test]
		void TestGet()
		{
			unsigned nSize = 16;
			EventHandlePool pool(nSize);			

			HandleObject handleObject;
			Assert::IsTrue(pool.GetHandleObject(handleObject), "The GetHandleObject() failed");

			Assert::AreEqual((unsigned) pool.GetSize(), nSize, "The pool size does not match the constructor");
			Assert::IsTrue(handleObject.IsValid(), "The returned handle object is not valid");			
		}

		[Test]
		void TestGetAndReturn()
		{
			unsigned nSize = 16;
			EventHandlePool pool(nSize);			

			HandleObject handleObject;
			Assert::IsTrue(pool.GetHandleObject(handleObject), "The GetHandleObject() failed");

			Assert::IsTrue(pool.ReturnHandleObject(handleObject), "The ReturnHandleObject() failed");

			Assert::AreEqual((unsigned) pool.GetSize(), nSize, "The pool size does not match the constructor");
			Assert::IsFalse(handleObject.IsValid(), "The returned handle object should not be valid");
		}

		[Test]
		void TestGetAllPlus1()
		{
			const unsigned nSize = 16;
			EventHandlePool pool(nSize);			

			HandleObject handleObjects[nSize];
					
			for (unsigned i = 0; i < nSize; i++)
			{
				Assert::IsTrue(pool.GetHandleObject(handleObjects[i]), "The GetHandleObject() failed");
				Assert::IsTrue(handleObjects[i].IsValid(), "The handle object is not valid");								
			}		

			HandleObject handleObject;

			Assert::IsFalse(pool.GetHandleObject(handleObject), "The GetHandleObject() succeeded");
			Assert::IsFalse(handleObject.IsValid(), "The returned handle object should not be valid");
		}

		[Test]
		void TestCookiesAreAllSame()
		{
			const unsigned nSize = 16;
			EventHandlePool pool(nSize);			

			HandleObject handleObjects[nSize];
		
			std::set<DWORD> slots;
			for (unsigned i = 0; i < nSize; i++)
			{
				Assert::IsTrue(pool.GetHandleObject(handleObjects[i]), "The GetHandleObject() failed");
				Assert::IsTrue(handleObjects[i].IsValid(), "The handle object is not valid");

				Assert::AreEqual((unsigned) handleObjects[0].GetCookie(), (unsigned) handleObjects[i].GetCookie(), 
					"The handle object cookie's should match");				
			}			
		}

		[Test]
		void TestSlotsAreUnique()
		{
			const unsigned nSize = 16;
			EventHandlePool pool(nSize);			

			HandleObject handleObjects[nSize];
		
			std::set<DWORD> slots;
			for (unsigned i = 0; i < nSize; i++)
			{
				Assert::IsTrue(pool.GetHandleObject(handleObjects[i]), "The GetHandleObject() failed");
				Assert::IsTrue(handleObjects[i].IsValid(), "The handle object is not valid");				

				slots.insert(handleObjects[i].GetSlot());
			}

			Assert::AreEqual(nSize, slots.size(), "The slots should all be unique");			
		}

		[Test]
		void TestPool1GetGetReturnGetGet()
		{
			const unsigned nSize = 1;
			EventHandlePool pool(nSize);						

			Assert::AreEqual((unsigned) pool.GetSize(), nSize, "The pool size does not match the constructor");			

			HandleObject handleObject1;
			Assert::IsTrue(pool.GetHandleObject(handleObject1), "The GetHandleObject() failed");
			Assert::IsTrue(handleObject1.IsValid(), "The returned handle object should be valid");

			HandleObject handleObject2;
			Assert::IsFalse(pool.GetHandleObject(handleObject2), "The GetHandleObject() succeeded");
			Assert::IsFalse(handleObject2.IsValid(), "The returned handle object should not be valid");

			Assert::IsTrue(pool.ReturnHandleObject(handleObject1), "The ReturnHandleObject() failed");
			Assert::IsFalse(handleObject1.IsValid(), "The handle object should not be valid");

			Assert::IsTrue(pool.GetHandleObject(handleObject2), "The GetHandleObject() failed");
			Assert::IsTrue(handleObject2.IsValid(), "The returned handle object should be valid");

			HandleObject handleObject3;
			Assert::IsFalse(pool.GetHandleObject(handleObject3), "The GetHandleObject() succeeded");
			Assert::IsFalse(handleObject3.IsValid(), "The returned handle object should not be valid");
		}

		[Test]
		void TestGetReturnInReverse()
		{
			const unsigned nSize = 16;
			EventHandlePool pool(nSize);			

			HandleObject handleObjects[nSize];
			
			for (unsigned i = 0; i < nSize; i++)
			{
				Assert::IsTrue(pool.GetHandleObject(handleObjects[i]), "The GetHandleObject() failed");
				Assert::IsTrue(handleObjects[i].IsValid(), "The handle object is not valid");

				Assert::AreEqual((unsigned) handleObjects[0].GetCookie(), (unsigned) handleObjects[i].GetCookie(), 
					"The handle object cookie's should match");
			}

			for (unsigned i = nSize; i > 0; i--)
			{
				unsigned index = i - 1;

				Assert::IsTrue(pool.ReturnHandleObject(handleObjects[index]), "The ReturnHandleObject() failed");
				Assert::IsFalse(handleObjects[index].IsValid(), "The returned handle object is valid");
			}

			HandleObject handleObject;

			Assert::IsTrue(pool.GetHandleObject(handleObject), "The GetHandleObject() failed");
			Assert::IsTrue(handleObject.IsValid(), "The handle object should be valid");
		}

		[Test]
		void TestReturnOtherHandleObject()
		{
			HANDLE hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
			HandleObject handleObject(hEvent);

			const unsigned nSize = 16;
			EventHandlePool pool(nSize);

			Assert::IsTrue(pool.ReturnHandleObject(handleObject), "ReturnHandleObject() returned false");
			Assert::IsFalse(handleObject.IsValid(), "IsValid() has returned true");
		}

	};
}
