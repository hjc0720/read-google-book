// =====================================================================================
// 
//       Filename:  RDKey.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  05/08/11 21:03:09
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Huang Jianchao (), hjc0720@gmail.com
//        Company:  
// 
// 
// =====================================================================================
#include "HVector3f.h"
#include "HVector4f.h"
#include "RDBaseKey.h"
#include "RDFileDataStream.h"

// =====================================================================================
template <typename Value>
Value RDLinearKey<Value>::Interpolation(double& dWeight, const RDKey<Value>& SecondKey)const
{
	return (1 - dWeight) * this->GetValue() + dWeight * SecondKey.GetValue();
}
// =====================================================================================
template <typename Value>
RDKey<Value>* RDKey<Value>::CreateKey(const Value& value, RDKeyType type /* = RDLineKey  */)
{
	switch(type)
	{
	case RDLineKey:
		return new RDLinearKey<Value>(value);
	}
	return nullptr;
}

template <typename Value>
RDKey<Value>* RDKey<Value>::CreateKey(RDKeyType type /* = RDLineKey  */)
{
	switch(type)
	{
	case RDLineKey:
		return new RDLinearKey<Value>();
	}
    return nullptr;
}

template <typename Value>
void RDKey<Value>::Serialize(RDJsonDataStream &buffer, Json::Value &parent)
{
    buffer.Serialize(parent,"value",m_KeyValue);
}
// =====================================================================================
template class RDKey<float3>;
template class RDKey<float>;
