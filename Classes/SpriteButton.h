#pragma once
#include "cocos2d.h"

namespace spriteButtonDefault {
	float pressingForce = 0.15f;
	float duration = 0.1f;
}

class SpriteButton : public cocos2d::Node {
	cocos2d::Sprite* sprite;
	cocos2d::EventListenerTouchOneByOne* listener;
	std::function<void(SpriteButton*)> callback;

	bool disabled;
	SpriteButton() {}
	bool init(cocos2d::Sprite* sprite, std::function<void(SpriteButton*)> function) {
		if (!Node::init()) {
			return false;
		}

		if (sprite == nullptr) {
			return false;
		}

		this->sprite = sprite;
		this->callback = function;
		this->disabled = false;

		this->setContentSize(sprite->getContentSize());
		this->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));
		sprite->setPosition(cocos2d::Vec2(this->getContentSize().width / 2, this->getContentSize().height / 2));

		this->addChild(sprite);

		listener = cocos2d::EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(true);
		listener->onTouchBegan = CC_CALLBACK_2(SpriteButton::touchIsDown, this);
		listener->onTouchEnded = CC_CALLBACK_2(SpriteButton::touchIsUp, this);
		cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

		return true;
	}
	cocos2d::Rect getRect() {
		cocos2d::Rect rect;

		rect.origin.x = sprite->getPosition().x - this->getContentSize().width / 2;
		rect.origin.y = sprite->getPosition().y - this->getContentSize().height / 2;

		rect.size = this->getContentSize();

		return rect;
	}

	bool touchIsDown(cocos2d::Touch* touch, cocos2d::Event* event) {

		if (disabled) {
			return false;
		}

		cocos2d::Vec2 touchPosInGlobalSpace = getPointInWorldSpace(touch->getLocation());
		cocos2d::Vec2 convertedTouchPos = this->convertToNodeSpace(touchPosInGlobalSpace);
		bool isContainsPoint = getRect().containsPoint(convertedTouchPos);
		
		if (isContainsPoint) {
			sprite->runAction(cocos2d::ScaleTo::create(spriteButtonDefault::duration, this->getScale() - spriteButtonDefault::pressingForce));
			return true;
		}
		return false;
	}
	bool touchIsUp(cocos2d::Touch* touch, cocos2d::Event* event) {
		sprite->runAction(cocos2d::ScaleTo::create(spriteButtonDefault::duration, this->getScale()));

		cocos2d::Vec2 touchPosInGlobalSpace = getPointInWorldSpace(touch->getLocation());
		cocos2d::Vec2 convertedTouchPos = this->convertToNodeSpace(touchPosInGlobalSpace);
		bool isContainsPoint = getRect().containsPoint(convertedTouchPos);

		if (isContainsPoint && !disabled) {
			if (callback) {
				callback(this);
			}
			return true;
		}
		return true;
	}

	cocos2d::Vec2 getPointInWorldSpace(cocos2d::Vec2 pos) {
		cocos2d::Vec2 centerPosition = cocos2d::Vec2(cocos2d::Director::getInstance()->getVisibleSize().width / 2, cocos2d::Director::getInstance()->getVisibleSize().height / 2);
		cocos2d::Vec2 cameraPosition = cocos2d::Director::getInstance()->getRunningScene()->getDefaultCamera()->getPosition();

		cocos2d::Vec2 cameraShift;
		cameraShift.x = centerPosition.x - cameraPosition.x;
		cameraShift.y = centerPosition.y - cameraPosition.y;

		return cocos2d::Vec2(pos.x - cameraShift.x, pos.y - cameraShift.y);
	}
public:
	static SpriteButton* create(cocos2d::Sprite* sprite) {
		SpriteButton* pRet = new(std::nothrow) SpriteButton;
		if (pRet->autorelease() && pRet->init(sprite, nullptr)) {
			pRet->retain();
			return pRet;
		}
		else {
			CC_SAFE_DELETE(pRet);
			return nullptr;
		}
	}

	static SpriteButton* create(cocos2d::Sprite* sprite, std::function<void(SpriteButton*)> callback) {
		SpriteButton* pRet = new(std::nothrow) SpriteButton;
		if (pRet->autorelease() && pRet->init(sprite, callback)) {
			pRet->retain();
			return pRet;
		}
		else {
			CC_SAFE_DELETE(pRet);
			return nullptr;
		}
	}
	void setContentSize(const Size& size) override {
		cocos2d::Node::setContentSize(size);
		sprite->setContentSize(size);
	}
	void setCallback(std::function<void(SpriteButton*)> callback) {
		this->callback = callback;
	}
	void setDisabled(bool disabled) {
		this->disabled = disabled;
		if (disabled) {
			sprite->setOpacity(100);
		}
		else {
			sprite->setOpacity(255);
		}
	}
};
