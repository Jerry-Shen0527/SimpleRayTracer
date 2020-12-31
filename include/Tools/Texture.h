#pragma once
/*
	pbrt source code is Copyright(c) 1998-2016
						Matt Pharr, Greg Humphreys, and Wenzel Jakob.

	This file is part of pbrt.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are
	met:

	- Redistributions of source code must retain the above copyright
	  notice, this list of conditions and the following disclaimer.

	- Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
	IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
	TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
	PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */
#include <Geometry/Interaction.h>

#include "Geometry/Transform.h"

class TextureMapping2D {
public:
	virtual Point2f Map(const SurfaceInteraction& si,
		Vector2f* dstdx, Vector2f* dstdy) const = 0;
	~TextureMapping2D();
};

class UVMapping2D : public TextureMapping2D {
public:
	// UVMapping2D Public Methods
	UVMapping2D(Float su = 1, Float sv = 1, Float du = 0, Float dv = 0);
	Point2f Map(const SurfaceInteraction& si, Vector2f* dstdx,
		Vector2f* dstdy) const;

private:
	const Float su, sv, du, dv;
};

class SphericalMapping2D : public TextureMapping2D {
public:
	// SphericalMapping2D Public Methods
	SphericalMapping2D(const Transform& WorldToTexture)
		: WorldToTexture(WorldToTexture) {}
	Point2f Map(const SurfaceInteraction& si, Vector2f* dstdx,
		Vector2f* dstdy) const;

private:
	Point2f sphere(const Point3f& P) const;
	const Transform WorldToTexture;
};

template <typename T>
class Texture {
public:
	// Texture Interface
	virtual T Evaluate(const SurfaceInteraction&) const = 0;
	virtual ~Texture() {}
};

template <typename T>
class ConstantTexture : public Texture<T> {
public:
	// ConstantTexture Public Methods
	ConstantTexture(const T& value) : value(value) {}
	T Evaluate(const SurfaceInteraction&) const { return value; }

private:
	T value;
};

template <typename T1, typename T2>
class ScaleTexture : public Texture<T2> {
public:
	// ScaleTexture Public Methods
	ScaleTexture(const std::shared_ptr<Texture<T1>>& tex1,
		const std::shared_ptr<Texture<T2>>& tex2)
		: tex1(tex1), tex2(tex2) {}
	T2 Evaluate(const SurfaceInteraction& si) const {
		return tex1->Evaluate(si) * tex2->Evaluate(si);
	}

private:
	// ScaleTexture Private Data
	std::shared_ptr<Texture<T1>> tex1;
	std::shared_ptr<Texture<T2>> tex2;
};

template <typename T>
class MixTexture : public Texture<T> {
public:
	// MixTexture Public Methods
	MixTexture(const std::shared_ptr<Texture<T>>& tex1,
		const std::shared_ptr<Texture<T>>& tex2,
		const std::shared_ptr<Texture<Float>>& amount)
		: tex1(tex1), tex2(tex2), amount(amount) {}
	T Evaluate(const SurfaceInteraction& si) const {
		T t1 = tex1->Evaluate(si), t2 = tex2->Evaluate(si);
		Float amt = amount->Evaluate(si);
		return (1 - amt) * t1 + amt * t2;
	}

private:
	std::shared_ptr<Texture<T>> tex1, tex2;
	std::shared_ptr<Texture<Float>> amount;
};

template <typename T>
class BilerpTexture : public Texture<T> {
public:
	// BilerpTexture Public Methods
	BilerpTexture(std::unique_ptr<TextureMapping2D> mapping, const T& v00,
		const T& v01, const T& v10, const T& v11)
		: mapping(std::move(mapping)), v00(v00), v01(v01), v10(v10), v11(v11) {}
	T Evaluate(const SurfaceInteraction& si) const {
		Vector2f dstdx, dstdy;
		Point2f st = mapping->Map(si, &dstdx, &dstdy);
		return (1 - st[0]) * (1 - st[1]) * v00 + (1 - st[0]) * (st[1]) * v01 +
			(st[0]) * (1 - st[1]) * v10 + (st[0]) * (st[1]) * v11;
	}

private:
	// BilerpTexture Private Data
	std::unique_ptr<TextureMapping2D> mapping;
	const T v00, v01, v10, v11;
};