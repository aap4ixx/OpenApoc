#pragma once
#include "framework/includes.h"
#include "framework/logger.h"
#include "framework/palette.h"
#include "framework/stage.h"
#include "game/state/gamestate.h"
#include "game/state/tileview/tile.h"
#include "library/sp.h"

namespace OpenApoc
{

class TileMap;
class Image;

class TileView : public Stage, public TileTransform
{
  protected:
	StageCmd stageCmd;
	TileMap &map;
	Vec3<int> isoTileSize;
	Vec2<int> stratTileSize;
	TileViewMode viewMode;

	bool scrollUp;
	bool scrollDown;
	bool scrollLeft;
	bool scrollRight;

	Vec2<int> dpySize;

	Colour strategyViewBoxColour;
	float strategyViewBoxThickness;

  public:
	int maxZDraw;
	Vec3<float> centerPos;
	Vec2<float> isoScrollSpeed;
	Vec2<float> stratScrollSpeed;

	Vec3<int> selectedTilePosition;
	sp<Image> selectedTileImageBack, selectedTileImageFront;
	sp<Palette> pal;

	TileView(TileMap &map, Vec3<int> isoTileSize, Vec2<int> stratTileSize,
	         TileViewMode initialMode);
	~TileView();

	Vec2<int> getScreenOffset() const;
	void setScreenCenterTile(Vec2<float> center);
	void setScreenCenterTile(Vec3<float> center);

	template <typename T> Vec2<T> tileToScreenCoords(Vec3<T> c, TileViewMode v) const
	{
		switch (v)
		{
			case TileViewMode::Isometric:
			{
				T x = (c.x * isoTileSize.x / 2) - (c.y * isoTileSize.x / 2);
				T y = (c.x * isoTileSize.y / 2) + (c.y * isoTileSize.y / 2) - (c.z * isoTileSize.z);
				return Vec2<T>{x, y};
			}
			case TileViewMode::Strategy:
			{
				return Vec2<T>{c.x * stratTileSize.x, c.y * stratTileSize.y};
			}
		}
		LogError("Invalid view mode");
		return {0, 0};
	}
	template <typename T> Vec2<T> tileToScreenCoords(Vec3<T> c) const
	{
		return this->tileToScreenCoords(c, this->viewMode);
	}

	Vec2<float> tileToScreenCoords(Vec3<float> coords) const override
	{
		return this->tileToScreenCoords(coords, this->viewMode);
	}

	template <typename T> Vec2<T> tileToOffsetScreenCoords(Vec3<T> c, TileViewMode v) const
	{
		return this->tileToScreenCoords(c, v) + Vec2<T>{this->getScreenOffset()};
	}
	template <typename T> Vec2<T> tileToOffsetScreenCoords(Vec3<T> c) const
	{
		return this->tileToScreenCoords(c, this->viewMode) + Vec2<T>{this->getScreenOffset()};
	}

	template <typename T> Vec3<T> screenToTileCoords(Vec2<T> screenPos, T z, TileViewMode v) const
	{
		switch (v)
		{
			case TileViewMode::Isometric:
			{
				screenPos.y += (z * isoTileSize.z);
				T y =
				    ((screenPos.y / (isoTileSize.y / 2)) - (screenPos.x / (isoTileSize.x / 2))) / 2;
				T x =
				    ((screenPos.y / (isoTileSize.y / 2)) + (screenPos.x / (isoTileSize.x / 2))) / 2;
				return Vec3<T>{x, y, z};
			}
			case TileViewMode::Strategy:
			{
				return Vec3<T>{screenPos.x / stratTileSize.x, screenPos.y / stratTileSize.y, z};
			}
		}
		LogError("Invalid view mode");
		return {0, 0, z};
	}
	template <typename T> Vec3<T> screenToTileCoords(Vec2<T> screenPos, T z) const
	{
		return this->screenToTileCoords(screenPos, z, this->viewMode);
	}

	Vec3<float> screenToTileCoords(Vec2<float> screenPos, float z) const override
	{
		return this->screenToTileCoords(screenPos, z, this->viewMode);
	}

	template <typename T>
	Vec3<T> offsetScreenToTileCoords(Vec2<T> screenPos, T z, TileViewMode v) const
	{
		return this->screenToTileCoords(screenPos - Vec2<T>{this->getScreenOffset()}, z, v);
	}
	template <typename T> Vec3<T> offsetScreenToTileCoords(Vec2<T> screenPos, T z) const
	{
		return this->screenToTileCoords(screenPos - Vec2<T>{this->getScreenOffset()}, z,
		                                this->viewMode);
	}

	// Stage control
	void Begin() override;
	void Pause() override;
	void Resume() override;
	void Finish() override;
	void EventOccurred(Event *e) override;
	void Render() override;
	bool IsTransition() override;

	virtual void setViewMode(TileViewMode newMode);
	virtual TileViewMode getViewMode() const;
};
}; // namespace OpenApoc
