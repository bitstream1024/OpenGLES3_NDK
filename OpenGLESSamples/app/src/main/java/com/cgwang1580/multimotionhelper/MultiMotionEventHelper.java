package com.cgwang1580.multimotionhelper;

import android.view.MotionEvent;
import com.cgwang1580.utils.LogUtils;
import java.util.ArrayList;
import java.util.List;

public class MultiMotionEventHelper {
    public static final String TAG = "MultiMotionEventHelper";
    // max point for fingers
    public static final int MAX_POINT_NUM = 10;
    /// space of record motion state for mesh move
    public static final long MIN_HANDLE_EVENT_TIME = 50; //ms
    private long mLastTime = 0;   // ms
    // if skip cal motion state for mesh move
    private boolean bSkipSetMotionState = false;
    public class MotionPoint {
        int id;
        float coord_x;
        float coord_y;
        float delta_x;
        float delta_y;

        private MotionPoint(int index, float x, float y) {
            id = index;
            coord_x = x;
            coord_y = y;
            delta_x = 0;
            delta_y = 0;
        }

        private MotionPoint(MotionPoint motionPoint) {
            id = motionPoint.id;
            coord_x = motionPoint.coord_x;
            coord_y = motionPoint.coord_y;
            delta_x = 0;
            delta_y = 0;
        }

        private void setZero() {
            coord_x = 0;
            coord_y = 0;
            delta_x = 0;
            delta_y = 0;
        }
    }
    private int mPointerNum;     // finger num
    /**
     * record each finger coordinate
     */
    private List <MotionPoint> mPointList = null;
    /**
     * record each finger point delta_x and delta_y
     */
    private MotionPoint [] mTempPointList = null;
    private MotionStateGL mMotionStateGL;           // motion state used to set to GLRender

    public MultiMotionEventHelper() {
        init();
    }
    private void init () {
        LogUtils.d(TAG, "init");
        mPointerNum = 0;
        mLastTime = 0;
        mPointList = new ArrayList<>();
        mTempPointList = new MotionPoint[MAX_POINT_NUM];
        mMotionStateGL = new MotionStateGL();
    }

    /**
     * brief multi finger control for touch thing, ACTION_DOWN and ACTION_UP will happen only first
     * finger or last finger touch or leave the screen, if ACTION_POINTER_DOWN and ACTION_POINTER_UP
     * happened, need find which finger
     * @param event MotionEvent
     */
    public void handleViewEvent(MotionEvent event) {
        //MyLog.d(TAG, "handleViewEvent");
        long eventTime = event.getEventTime();
        float x_now = event.getX();
        float y_now = event.getY();
        int actionId = 0;
        int pointId = 0;
        int pointNum = 0;
        int pointArrayNum = mPointList.size();
        try {
            mPointerNum = event.getPointerCount();
            //MyLog.d(TAG, "onTouchEvent mPointerNum = " + mPointerNum);
            int actionMask = event.getActionMasked();
            LogUtils.d(TAG, "onTouchEvent actionMask = " + actionMask);
            switch (actionMask) {
                case MotionEvent.ACTION_DOWN:
                    LogUtils.d(TAG, "onTouchEvent ACTION_DOWN x_ori = x_ori = " + x_now + ", y_ori = " + y_now);
                    actionId = event.getActionIndex();
                    pointId = event.getPointerId(actionId);
                    if (0 == pointArrayNum) {
                        MotionPoint motionPoint = new MotionPoint(pointId, x_now, y_now);
                        mPointList.add(motionPoint);
                        mTempPointList [0] = motionPoint;
                    }
                    break;
                case MotionEvent.ACTION_UP:
                    // the last pointer is leave, so clear mPointList
                    mPointList.clear();
                    for (int i = 0; i < mTempPointList.length; ++i) {
                        if (null != mTempPointList[i])  mTempPointList[i].setZero();
                    }
                    break;
                case MotionEvent.ACTION_MOVE:
                    /*if (Math.abs(x_now - x_now) > CommonDefine.VIEW_MOVE_THRESH_X || Math.abs(y_now - y_now) > CommonDefine.VIEW_MOVE_THRESH_Y) {
                        //MyLog.d(TAG, "onTouchEvent ACTION_MOVE x_ori = x_now = " + x_now + ", y_now = " + y_now);
                    }*/
                    if (0 == mLastTime) {
                        mLastTime = eventTime;
                        bSkipSetMotionState = true;
                        break;
                    }
                    if ((eventTime - mLastTime) < MIN_HANDLE_EVENT_TIME) {
                        bSkipSetMotionState = true;
                        break;
                    }
                    mLastTime = eventTime;
                    for (int i = 0; i < mPointerNum; ++i)
                    {
                        x_now = event.getX(i);
                        y_now = event.getY(i);
                        mTempPointList[i].id = i;
                        mTempPointList[i].delta_x = x_now - mPointList.get(i).coord_x;
                        mTempPointList[i].delta_y = -(y_now - mPointList.get(i).coord_y);  // for y coordinate is converse
                        mPointList.set(i, new MotionPoint(i, x_now, y_now));
                    }
                    bSkipSetMotionState = false;
                    break;
                case MotionEvent.ACTION_POINTER_DOWN:
                    actionId = event.getActionIndex();
                    pointId = event.getPointerId(actionId);
                    x_now = event.getX(pointId);
                    y_now = event.getY(pointId);
                    LogUtils.d(TAG, "onTouchEvent ACTION_POINTER_DOWN pointId = " + pointId +
                            " actionId = " + actionId + " x_now = " + x_now + " y_now" + y_now);
                    MotionPoint motionPoint = new MotionPoint(pointId, x_now, y_now);
                    if (pointId + 1 > mPointList.size()) {
                        // pointer is not exit, add this pointer
                        mPointList.add(new MotionPoint(pointId, x_now, y_now));
                    } else {
                        // pointer is exit
                        mPointList.set(pointId, motionPoint);
                    }
                    mTempPointList [pointId] = motionPoint;
                    break;
                case MotionEvent.ACTION_POINTER_UP:
                    actionId = event.getActionIndex();
                    pointId = event.getPointerId(actionId);
                    LogUtils.d(TAG, "onTouchEvent ACTION_POINTER_UP pointId = " + pointId + " actionId = " + actionId);
                    mPointList.get(pointId).setZero();
                    mTempPointList [pointId].setZero();
                    break;
                default:
                    break;
            }
        } catch (RuntimeException e) {
            e.printStackTrace();
        }
    }

    public MotionStateGL getMotionStateGL (int windowWidth, int windowHeight) {
        mMotionStateGL.setZero();
        if (2 == mPointerNum && 0 != windowWidth && 0 != windowHeight && !bSkipSetMotionState) {
            LogUtils.d(TAG, "mTempPointList (" + mTempPointList[0].delta_x + " " + mTempPointList[0].delta_y
                    + " " + mTempPointList[1].delta_x + " " + mTempPointList[1].delta_y);
            if (mTempPointList[0].delta_x * mTempPointList[1].delta_x > 0 && mTempPointList[0].delta_y * mTempPointList[1].delta_y > 0) {
                // means translate
                mMotionStateGL.mMotionType = mMotionStateGL.eMOTION_TRANSLATE;
                mMotionStateGL.translate_x = mTempPointList[0].delta_x/(windowWidth/2.f);
                mMotionStateGL.translate_y = mTempPointList[0].delta_y/(windowHeight/2.f);
            }
        }
        return mMotionStateGL;
    }
}
