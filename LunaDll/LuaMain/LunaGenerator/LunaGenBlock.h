#ifndef LunaGenBlock_hhhhh
#define LunaGenBlock_hhhhh

#include "../../SMBXInternal/Blocks.h"
#include "LunaGenHelperUtils.h"

#include <luabind/luabind.hpp>

template<typename T>
inline auto GenerateBindingBlock()
{
    typedef typename T::internal_class internal_class;
    return LunaGen::LunaGenHelper<T>::defClass()
        .property("Slippery", &T::Getter<decltype(internal_class::Slippery), &internal_class::Slippery>, &T::Setter<decltype(internal_class::Slippery), &internal_class::Slippery>)
        .property("slippery", &T::Getter<decltype(internal_class::Slippery), &internal_class::Slippery>, &T::Setter<decltype(internal_class::Slippery), &internal_class::Slippery>)
        .property("RepeatingHits", &T::Getter<decltype(internal_class::RepeatingHits), &internal_class::RepeatingHits>, &T::Setter<decltype(internal_class::RepeatingHits), &internal_class::RepeatingHits>)
        .property("BlockType2", &T::Getter<decltype(internal_class::BlockType2), &internal_class::BlockType2>, &T::Setter<decltype(internal_class::BlockType2), &internal_class::BlockType2>)
        .property("ContentIDRelated", &T::Getter<decltype(internal_class::ContentIDRelated), &internal_class::ContentIDRelated>, &T::Setter<decltype(internal_class::ContentIDRelated), &internal_class::ContentIDRelated>)
        .property("pHitEventName", &T::Getter<decltype(internal_class::pHitEventName), &internal_class::pHitEventName>, &T::Setter<decltype(internal_class::pHitEventName), &internal_class::pHitEventName>)
        .property("pDestroyEventName", &T::Getter<decltype(internal_class::pDestroyEventName), &internal_class::pDestroyEventName>, &T::Setter<decltype(internal_class::pDestroyEventName), &internal_class::pDestroyEventName>)
        .property("pNoMoreObjInLayerEventName", &T::Getter<decltype(internal_class::pNoMoreObjInLayerEventName), &internal_class::pNoMoreObjInLayerEventName>, &T::Setter<decltype(internal_class::pNoMoreObjInLayerEventName), &internal_class::pNoMoreObjInLayerEventName>)
        .property("pLayerName", &T::Getter<decltype(internal_class::pLayerName), &internal_class::pLayerName>, &T::Setter<decltype(internal_class::pLayerName), &internal_class::pLayerName>)
        .property("layerName", &T::Getter<decltype(internal_class::pLayerName), &internal_class::pLayerName>, &T::Setter<decltype(internal_class::pLayerName), &internal_class::pLayerName>)
        .property("IsHidden", &T::Getter<decltype(internal_class::IsHidden), &internal_class::IsHidden>, &T::Setter<decltype(internal_class::IsHidden), &internal_class::IsHidden>)
        .property("invisible", &T::Getter<decltype(internal_class::IsHidden), &internal_class::IsHidden>, &T::Setter<decltype(internal_class::IsHidden), &internal_class::IsHidden>)
        .property("isHidden", &T::Getter<decltype(internal_class::IsHidden), &internal_class::IsHidden>, &T::Setter<decltype(internal_class::IsHidden), &internal_class::IsHidden>)
        .property("BlockType", &T::Getter<decltype(internal_class::BlockType), &internal_class::BlockType>, &T::Setter<decltype(internal_class::BlockType), &internal_class::BlockType>)
        .property("id", &T::Getter<decltype(internal_class::BlockType), &internal_class::BlockType>, &T::Setter<decltype(internal_class::BlockType), &internal_class::BlockType>)
        .property("x", &T::MomentumGetter<&internal_class::momentum, &Momentum::x>, &T::MomentumSetter<&internal_class::momentum, &Momentum::x>)
        .property("y", &T::MomentumGetter<&internal_class::momentum, &Momentum::y>, &T::MomentumSetter<&internal_class::momentum, &Momentum::y>)
        .property("width", &T::MomentumGetter<&internal_class::momentum, &Momentum::width>, &T::MomentumSetter<&internal_class::momentum, &Momentum::width>)
        .property("height", &T::MomentumGetter<&internal_class::momentum, &Momentum::height>, &T::MomentumSetter<&internal_class::momentum, &Momentum::height>)
        .property("speedX", &T::MomentumGetter<&internal_class::momentum, &Momentum::speedX>, &T::MomentumSetter<&internal_class::momentum, &Momentum::speedX>)
        .property("speedY", &T::MomentumGetter<&internal_class::momentum, &Momentum::speedY>, &T::MomentumSetter<&internal_class::momentum, &Momentum::speedY>)
        .property("ContentsID", &T::Getter<decltype(internal_class::ContentsID), &internal_class::ContentsID>, &T::Setter<decltype(internal_class::ContentsID), &internal_class::ContentsID>)
        .property("contentID", &T::Getter<decltype(internal_class::ContentsID), &internal_class::ContentsID>, &T::Setter<decltype(internal_class::ContentsID), &internal_class::ContentsID>)
        .property("BeingHitStatus1", &T::Getter<decltype(internal_class::BeingHitStatus1), &internal_class::BeingHitStatus1>, &T::Setter<decltype(internal_class::BeingHitStatus1), &internal_class::BeingHitStatus1>)
        .property("BeingHitTimer", &T::Getter<decltype(internal_class::BeingHitTimer), &internal_class::BeingHitTimer>, &T::Setter<decltype(internal_class::BeingHitTimer), &internal_class::BeingHitTimer>)
        .property("HitOffset", &T::Getter<decltype(internal_class::HitOffset), &internal_class::HitOffset>, &T::Setter<decltype(internal_class::HitOffset), &internal_class::HitOffset>)
        .property("IsInvisible2", &T::Getter<decltype(internal_class::IsInvisible2), &internal_class::IsInvisible2>, &T::Setter<decltype(internal_class::IsInvisible2), &internal_class::IsInvisible2>)
        .property("IsInvisible3", &T::Getter<decltype(internal_class::IsInvisible3), &internal_class::IsInvisible3>, &T::Setter<decltype(internal_class::IsInvisible3), &internal_class::IsInvisible3>)
;}

#endif