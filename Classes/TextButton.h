#pragma once
#include "cocos2d.h"
#include "ui/UIScale9Sprite.h"

namespace textButtonDefault {
	std::string spritePath = "Square.png";
	std::string fontPath = "fonts/arial.ttf";
	cocos2d::Rect rect = cocos2d::Rect(20, 20, 60, 60);
	cocos2d::Color3B textColor = cocos2d::Color3B::BLACK;
	float pressingForce = 0.15f;
	float duration = 0.1f;
}

class TextButton : public cocos2d::Node {
	cocos2d::ui::Scale9Sprite* sprite;
	cocos2d::Label* label;
	cocos2d::Size size;
	cocos2d::EventListenerTouchOneByOne* listener;
	std::function<void(TextButton*)> callback;
	
	float textDistanceK;
	bool disabled;
	TextButton() {}
	bool init(std::string titleText, cocos2d::Size size, std::function<void(TextButton*)> function) {
		if (!Node::init()) {
			return false;
		}

		this->callback = function;
		this->disabled = false;


		sprite = cocos2d::ui::Scale9Sprite::create(textButtonDefault::spritePath);
		sprite->setScale9Enabled(true);
		sprite->setCapInsets(textButtonDefault::rect);
		sprite->setContentSize(size);



		label = cocos2d::Label::createWithTTF(titleText, textButtonDefault::fontPath, sprite->getContentSize().height * 0.8);
		label->setPosition(cocos2d::Vec2(sprite->getContentSize().width / 2, sprite->getContentSize().height / 2));
		label->setColor(textButtonDefault::textColor);

		textDistanceK = 0.2;
		while (label->getBoundingBox().size.width > size.width * (1 - textDistanceK)) {
			cocos2d::TTFConfig config = label->getTTFConfig();
			config.fontSize -= 0.01;
			label->setTTFConfig(config);
		}

		sprite->addChild(label);



		this->setContentSize(size);
		this->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));
		sprite->setPosition(cocos2d::Vec2(this->getContentSize().width / 2, this->getContentSize().height / 2));

		this->addChild(sprite);

		listener = cocos2d::EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(true);
		listener->onTouchBegan = CC_CALLBACK_2(TextButton::touchIsDown, this);
		listener->onTouchEnded = CC_CALLBACK_2(TextButton::touchIsUp, this);
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
			sprite->runAction(cocos2d::ScaleTo::create(textButtonDefault::duration, this->getScale() - textButtonDefault::pressingForce));
			return true;
		}
		return false;
	}
	bool touchIsUp(cocos2d::Touch* touch, cocos2d::Event* event) {
		sprite->runAction(cocos2d::ScaleTo::create(textButtonDefault::duration, this->getScale()));

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
	static TextButton* create(std::string title, cocos2d::Size size) {
		TextButton* pRet = new(std::nothrow) TextButton;
		if (pRet->autorelease() && pRet->init(title, size, nullptr)) {
			pRet->retain();
			return pRet;
		}
		else {
			CC_SAFE_DELETE(pRet);
			return nullptr;
		}
	}

	static TextButton* create(std::string title, cocos2d::Size size, std::function<void(TextButton*)> callback) {
		TextButton* pRet = new(std::nothrow) TextButton;
		if (pRet->autorelease() && pRet->init(title, size, callback)) {
			pRet->retain();
			return pRet;
		}
		else {
			CC_SAFE_DELETE(pRet);
			return nullptr;
		}
	}
	void setContentSize(const cocos2d::Size& size) override {
		cocos2d::Node::setContentSize(size);
		sprite->setContentSize(size);
		label->setPosition(cocos2d::Vec2(sprite->getContentSize().width / 2, sprite->getContentSize().height / 2));
		/*TTFConfig config = label->getTTFConfig();
		config.fontSize = sprite->getContentSize().height * 0.8;
		label->setTTFConfig(config);
		while (label->getBoundingBox().size.width > size.width * (1 - textDistanceK)) {
			config.fontSize -= 0.01;
			label->setTTFConfig(config);
		}*/
	}
	void setCallback(std::function<void(TextButton*)> callback) {
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
