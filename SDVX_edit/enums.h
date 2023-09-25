#pragma once
enum CommandType
{
	INVALID,
	SPIN_R,
	SPIN_L,
	HALF_SPIN_R,
	HALF_SPIN_L,
	SWING_R,
	SWING_L,
	TEMPO_CHANGE,
	BEAT_CHANGE,
	SLAM_VOLUME,
	SLAM_SETTING,
	PEAK_FILTER_GAIN,
	STOP,
	TILT,
	ZOOM_TOP,
	ZOOM_BOTTOM,
	ZOOM_SIDE,
	CENTER_SPLIT,
	WIDE_LASER_L,
	WIDE_LASER_R,
	FX_L,
	FX_R,
	FX_L_EFFECT,
	FX_R_EFFECT,
	FILTER_TYPE,
	//no documentation for this
	TICKRATE_OFFSET
};

enum Option
{
	TITLE,
	TITLE_IMG,
	ARTIST,
	ARTIST_IMG,
	EFFECT,
	JACKET,
	ILLUSTRATOR,
	DIFFICULTY,
	LEVEL,
	TEMPO,
	TEMPO_MAX,
	BEAT,
	SONG_FILE,
	VOL,
	OFFSET,
	BACKGROUND,
	LAYER,
	PREVIEW_OFFSET,
	PREVIEW_LENGTH,
	TOTAL,
	SLAMVOLUME,
	//FILTER_TYPE,
	PFILTER_GAIN,
	PFILTER_DELAY,
	VIDEO_FILE,
	VIDEO_OFFSET,
	VERSION,
	INFO
};

enum ToolType
{
	LASER_L = 0,
	LASER_R = 1,
	BT,
	FX,
};

enum LaserType
{
	LASER_BODY,
	LASER_TAIL,
	LASER_HEAD,
	LASER_NONE,
};