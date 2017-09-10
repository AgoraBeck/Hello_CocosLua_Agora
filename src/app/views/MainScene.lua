cc.exports.agoraVer  = Agora:getVersion()

local VIDEOPROFILE_360P = 30

local MainScene = class("MainScene", cc.load("mvc").ViewBase)

function MainScene:onCreate()
	local leftPadding = 10

	self._localVideoSprite = nil
	self._remoteVideoSprite = nil

    -- add Branch image
    display.newSprite("HelloAgora.png")
    	:move(display.cx + display.width*2/5 , display.cy + display.height * 3/8)
        :addTo(self)
        :setScale(0.5)

    -- add Hello label
    cc.Label:createWithSystemFont("Hello Cocos2d Lua", "Arial", 40)
        :move(display.cx, display.cy + display.height*2/5)
        :addTo(self)

    -- add labelCid 
    self.lableCname = cc.Label:createWithSystemFont("频道 ID:", "Arial", 15)
    self.lableCname:setPosition(display.cx/11, display.cy + display.height *3/7);
    self:addChild(self.lableCname, 0 );

    -- add EditBox 
    self.ChannelID = cc.EditBox:create(cc.size(150, 40), "res/TextBox.png");
    self.ChannelID:setTextHorizontalAlignment(cc.TEXT_ALIGNMENT_LEFT);
    self.ChannelID:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE);
    self.ChannelID:setPosition(display.cx/4 + 3*leftPadding, display.cy + display.height *3/7)
    self.ChannelID:setMaxLength(10);
    self:addChild(self.ChannelID, 0 );
    self.ChannelID:setText("666")

     -- add labelUid 
    self.lableUid = cc.Label:createWithSystemFont("用户 ID:", "Arial", 15)
    self.lableUid:setPosition(display.cx/11, display.cy + display.height *3/7 - 50);
    self:addChild(self.lableUid, 0);

     -- add EditBox 
    self.CnameID = cc.EditBox:create(cc.size(150, 40), "res/TextBox.png");
    self.CnameID:setTextHorizontalAlignment(cc.TEXT_ALIGNMENT_LEFT);
    self.CnameID:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE);
    self.CnameID:setPosition(display.cx/4 + 3*leftPadding, display.cy + display.height *3/7 - 50 )
    self.CnameID:setMaxLength(10);
    self:addChild(self.CnameID, 0 );
    self.CnameID:setText("")

    -- add Join Button
    local joinBtn = ccui.Button:create()
    joinBtn:setPosition(display.cx/4 + leftPadding, display.cy + display.height/5)
	joinBtn:setTouchEnabled(true)
	joinBtn:loadTextures("res/Button.png", "res/ButtonPressed.png", "res/ButtonPressed.png")
	joinBtn:setTitleText("Join Channel")
	self:addChild(joinBtn, 1)
	joinBtn:addClickEventListener(function(sender ) 
        print("== joinBtn  ==");
        self:JoinChannelClicked( );
    end );

	-- add Leave Button
	local LeaveBtn = ccui.Button:create()
    LeaveBtn:setPosition(display.cx/4 + leftPadding, display.cy)
	LeaveBtn:setTouchEnabled(true)
	LeaveBtn:loadTextures("res/Button.png", "res/ButtonPressed.png", "res/ButtonPressed.png")
	LeaveBtn:setTitleText("Leave Channel")
	self:addChild(LeaveBtn, 1)
	LeaveBtn:addClickEventListener(function(sender ) 
        self:LeaveChannelClicked();
    end );

	-- add “Colse” icon
    local closeBtn = ccui.Button:create()
    closeBtn:setPosition(display.width - 13, 13);    
    closeBtn:setTouchEnabled(true)
    closeBtn:loadTextures("res/CloseNormal.png", "res/CloseSelected.png", "res/CloseSelected.png")
    self:addChild(closeBtn, 1)
    closeBtn:addClickEventListener(function(sender) 
        print("== menuClose  ==");
        self:menuClose();
    end);

	-- add Tip icon
	self.lableTips = cc.Label:createWithSystemFont("1111 ", "Arial", 15)
    self.lableTips:move(display.cx , display.cy + display.height/3)
    self.lableTips:setHorizontalAlignment( cc.TEXT_ALIGNMENT_LEFT )
    self:addChild(self.lableTips, 1)

	-- setCallback
	Agora.setCallback("onJoinChannelSuccess", function(channel, uid, elapsed)
	self:setTips("[Lua] onJoinChannelSuccess ChannelID:"..channel.." ,uid:" ..uid) 
	if not self._localVideoSprite then

		local videospr = Agora.getLocalSprite()
        self._localVideoSprite = videospr
        self._localVideoSprite:setPosition(cc.p(display.cx - 100,display.cy))
        self.addChild(self, self._localVideoSprite, 1)
        end
	end)

    Agora.setCallback("onFirstRemoteVideoDecoded", function(uid, width, height, elapsed)
        print("[Lua] onFirstRemoteVideoDecoded " .. uid)
        if not self._remoteVideoSprite then
            print("[Lua] onFirstRemoteVideoDecoded  here ")
            local rVideospr = Agora.getRemoteSprite(uid)  
            if rVideospr  == nil then
                print("rVideospr is null")
            end
            self._remoteVideoSprite = rVideospr
            self._remoteVideoSprite:setPosition(cc.p(display.cx + 160,display.cy))
            self.addChild(self, self._remoteVideoSprite, 1)
        end
    end)

    Agora.setCallback("onLeaveChannel", function(stats)
        self:setTips("[Lua] onLeaveChannel Success ") 
        self:clearVideoSprites()
    end)    

    Agora.setCallback("onUserOffline", function(uid, reason)
        print("[Lua] onUserOffline " ..uid .. reason)
        if self._remoteVideoSprite then
            self._remoteVideoSprite:removeFromParent()
            self._remoteVideoSprite = nil
        end
    end)

    Agora.setCallback("onUserEnableVideo", function(uid, enabled)
        print("[Lua] onUserEnableVideo " ..uid)
        if self._remoteVideoSprite then
            self._remoteVideoSprite:setVisible(enabled)
        end
    end)

    Agora.setCallback("onUserMuteVideo", function(uid, enabled)
        print("[Lua] onUserMuteVideo " ..uid)
        if self._remoteVideoSprite then
            self._remoteVideoSprite:setVisible(not enabled)          
        end
    end)
end

function MainScene:JoinChannelClicked()
    --set tips
    self:setTips( "[Lua] Agora SDK Version:" ..cc.exports.agoraVer)

	local ChannelID = self.ChannelID:getText()

    local CnameID = self.CnameID:getText()

    if not self:isValidID(ChannelID)  then 
        self:setTips("[Lua] 频道ID无效")
        return 
    end

    if not  self:isValidUID(CnameID) then 
        self:setTips("[Lua] 用户ID无效。请输入数字")
        return 
    end
    
    Agora:joinChannel(ChannelID, CnameID, true, VIDEOPROFILE_360P, "HelloLua")
    

end

function MainScene:LeaveChannelClicked()
    print("[Lua] The LeaveChannelClicked ")
    Agora:leaveChannel()
    
end


function MainScene:menuClose()
    print("[Lua] The menuClose ")
    Agora:leaveChannel()
    self:clearVideoSprites() 
    Agora:exit()
end

function MainScene:clearVideoSprites()
    if self._localVideoSprite then
        self._localVideoSprite:removeFromParent()
        self._localVideoSprite = nil
    end
        
    if self._remoteVideoSprite then
        self._remoteVideoSprite:removeFromParent()
        self._remoteVideoSprite = nil
    end

end

function MainScene:isValidID( id )
    if string.len(id)  == 0 then 
        return false 
    end

    local tempstr  = id
    tempstr = string.gsub( tempstr, "_","")
    tempstr = string.gsub( tempstr, "%a","")
    tempstr = string.gsub(tempstr, "%d","" )

    if string.len( tempstr ) == 0 then 
        return true
    else 
        return false
    end
end

function MainScene:isValidUID( id )
    if string.len(id)  == 0 then 
        return false 
    end

    local tempstr  = id
    tempstr = string.gsub(tempstr, "%d","" )

    if string.len( tempstr ) == 0 then 
        return true
    else 
        return false
    end
end

function MainScene:setTips( tips )
    self.lableTips:setString( tips )
end

return MainScene
