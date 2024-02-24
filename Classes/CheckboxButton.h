#pragma once

#include "cocos2d.h"

class CheckboxButton : public cocos2d::Node {
	std::string filepathNormal;
	std::string filepathSelected;
	cocos2d::Texture2D* normalTexture;
	cocos2d::Texture2D* selectedTexture;
	cocos2d::Sprite* sprite;
	bool selected;
	bool disabled;
	float duration = 0.05f;
	float pressingForce = 0.15f;
	float startScale;
	std::function<void(CheckboxButton*)> callback;
	cocos2d::Vec2 getPointInWorldSpace(cocos2d::Vec2 pos) {
		cocos2d::Vec2 centerPosition = cocos2d::Vec2(cocos2d::Director::getInstance()->getVisibleSize().width / 2, cocos2d::Director::getInstance()->getVisibleSize().height / 2);
		cocos2d::Vec2 cameraPosition = cocos2d::Director::getInstance()->getRunningScene()->getDefaultCamera()->getPosition();

		cocos2d::Vec2 cameraShift;
		cameraShift.x = centerPosition.x - cameraPosition.x;
		cameraShift.y = centerPosition.y - cameraPosition.y;

		return cocos2d::Vec2(pos.x - cameraShift.x, pos.y - cameraShift.y);
	}
	cocos2d::Rect getRect() {
		cocos2d::Rect rect;

		rect.origin.x = sprite->getPosition().x - this->getContentSize().width / 2 + cocos2d::Director::getInstance()->getVisibleOrigin().x;
		rect.origin.y = sprite->getPosition().y - this->getContentSize().height / 2 + cocos2d::Director::getInstance()->getVisibleOrigin().y;

		rect.size = this->getContentSize();

		return rect;
	}
	bool init(std::string filepathNormal, std::string filepathSelected, std::function<void(CheckboxButton*)> callback) {

		this->filepathNormal = filepathNormal;
		this->filepathSelected = filepathSelected;
		this->callback = callback;
		this->disabled = false;

		if (cocos2d::Sprite::create(filepathNormal) == nullptr || cocos2d::Sprite::create(filepathSelected) == nullptr) {
			return false;
		}

		normalTexture = cocos2d::Director::getInstance()->getTextureCache()->addImage(filepathNormal);
		selectedTexture = cocos2d::Director::getInstance()->getTextureCache()->addImage(filepathSelected);

		sprite = cocos2d::Sprite::createWithTexture(normalTexture);


		cocos2d::Size size = normalTexture->getContentSize();

		this->setContentSize(size);
		this->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));
		this->startScale = sprite->getScale();
		this->setCascadeColorEnabled(true);
		this->addChild(sprite);

		sprite->setPosition(cocos2d::Vec2(size.width / 2, size.height / 2));

		cocos2d::EventListenerTouchOneByOne* listener = cocos2d::EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(true);
		listener->onTouchBegan = CC_CALLBACK_2(CheckboxButton::touchIsDown, this);
		listener->onTouchEnded = CC_CALLBACK_2(CheckboxButton::touchIsUp, this);
		cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);



		return true;
	}
	bool touchIsDown(cocos2d::Touch* touch, cocos2d::Event* event) {
		if (disabled) {
			return false;
		}
		cocos2d::Vec2 touchPos = sprite->getParent()->convertToNodeSpace(getPointInWorldSpace(touch->getLocation()));
		bool isIntersects = getRect().containsPoint(touchPos);
		sprite->cleanup();
		if (isIntersects) {
			sprite->runAction(cocos2d::ScaleTo::create(duration, (startScale - pressingForce)));
			return true;
		}
		return false;
	}

	bool touchIsUp(cocos2d::Touch* touch, cocos2d::Event* event) {
		cocos2d::Vec2 touchPos = sprite->getParent()->convertToNodeSpace(getPointInWorldSpace(touch->getLocation()));
		bool isIntersects = getRect().containsPoint(touchPos);
		sprite->cleanup();
		if (isIntersects) {
			sprite->runAction(cocos2d::ScaleTo::create(duration, startScale));

			selected = !selected;

			if (selected) {
				sprite->setTexture(normalTexture);
			}
			else {
				sprite->setTexture(selectedTexture);
			}
			if (callback) {
				callback(this);
			}
			return true;
		}
		return false;
	}
public:
	static CheckboxButton* create(std::string normal, std::string selected) {
		CheckboxButton* pRet = new (std::nothrow) CheckboxButton;

		if (pRet->autorelease() && pRet->init(normal, selected, nullptr)) {
			pRet->retain();
			return pRet;
		}
		else {
			CC_SAFE_DELETE(pRet);
		}
		return nullptr;
	}

	static CheckboxButton* create(std::string normal, std::string selected, std::function<void(CheckboxButton*)> callback) {
		CheckboxButton* pRet = new (std::nothrow) CheckboxButton;

		if (pRet->autorelease() && pRet->init(normal, selected, callback)) {
			pRet->retain();
			return pRet;
		}
		else {
			CC_SAFE_DELETE(pRet);
		}
		return nullptr;
	}
	void setContentSize(const cocos2d::Size& size) override {
		cocos2d::Node::setContentSize(size);
		sprite->setContentSize(size);
	}
	void setCallback(std::function<void(CheckboxButton*)> callback) {
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
	cocos2d::Sprite* getSprite() { return sprite; }
};
