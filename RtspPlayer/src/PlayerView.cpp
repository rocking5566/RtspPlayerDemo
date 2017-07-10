#include "PlayerView.h"
#include "Util.h"
#include "opencv2/opencv.hpp"
#include <QInputDialog>
#include <string>
#include <QThread>

#define VIDEO_WIDTH 1024
#define VIDEO_HEIGHT 578

using namespace std;
using namespace cv;

CPlayerView::CPlayerView(QWidget *parent)
    : QMainWindow(parent)
    , m_vlcInstance(NULL)
    , m_vlcMediaPlayer(NULL)
    , m_videobuf(new char[(VIDEO_WIDTH * VIDEO_HEIGHT) << 2])
{
    m_ui.setupUi(this);
    connect(m_ui.btnOpenVideo, SIGNAL(clicked()), this, SLOT(OnOpenVideoBtnClick()));
    connect(m_ui.btnPlay, SIGNAL(clicked()), this, SLOT(OnPlay()));
    connect(m_ui.btnPause, SIGNAL(clicked()), this, SLOT(OnPause()));

    char const* vlc_args[] =
    {
        "-I",
        "dummy",
        "--ignore-config",
    };

    m_vlcInstance = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
    m_vlcMediaPlayer = libvlc_media_player_new (m_vlcInstance);
    memset(m_videobuf, 0, (VIDEO_WIDTH * VIDEO_HEIGHT) << 2);
}

CPlayerView::~CPlayerView()
{
    libvlc_media_player_stop(m_vlcMediaPlayer);
    libvlc_media_player_release(m_vlcMediaPlayer);
    libvlc_release(m_vlcInstance);
}

char* CPlayerView::GetVideoBuf() const
{
    return m_videobuf;
}

void CPlayerView::OnOpenVideoBtnClick()
{
    bool bOk = false;
    string sRtsp = QInputDialog::getText(this, tr("Open RTSP"),
        tr("RTSP url:"), QLineEdit::Normal,
        "rtsp://127.0.0.1:8554/test.mkv", &bOk).toStdString();

    if (bOk)
    {
        OpenVideo(sRtsp.c_str());
    }
}

void CPlayerView::OpenVideo(const char* rtsp)
{
    OnPause();

    libvlc_media_t* pvlcMedia = libvlc_media_new_location(m_vlcInstance, rtsp);
    libvlc_media_player_set_media(m_vlcMediaPlayer, pvlcMedia);
    
    libvlc_media_track_info_t *tracks = NULL;
    libvlc_media_parse(pvlcMedia);
    libvlc_media_get_tracks_info(pvlcMedia, &tracks);
    
    libvlc_media_release(pvlcMedia);

    libvlc_video_set_callbacks(m_vlcMediaPlayer, PreDecode_cb, HandleStream_cb, Render_cb, this);
    libvlc_video_set_format(m_vlcMediaPlayer, "RV32", VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_WIDTH << 2);

    OnPlay();
}

void CPlayerView::OnPlay()
{
    libvlc_media_player_play(m_vlcMediaPlayer);

    // [TODO] - Error handle
    // watch libvlc_media_player_get_state() here
    // Once the libvlc_state_t become libvlc_Playing
    // Call libvlc_media_get_tracks_info to get the resolution
    // Then Move libvlc_video_set_callbacks() and libvlc_video_set_format to here
}

void CPlayerView::OnPause()
{
    libvlc_media_player_pause(m_vlcMediaPlayer);
}

void* CPlayerView::PreDecode_cb(void *opaque, void **planes)
{
    CPlayerView* obj = (CPlayerView*)opaque;
    *planes = obj->GetVideoBuf();

    return obj->GetVideoBuf();
}

void CPlayerView::HandleStream_cb(void *opaque, void *picture, void *const *planes)
{
    Q_UNUSED(opaque);
    Q_UNUSED(picture);
    Q_UNUSED(planes);
    // TODO - image processing
    // Mat img(Size(VIDEO_WIDTH, VIDEO_HEIGHT), CV_8UC4, picture);
}

void CPlayerView::Render_cb(void *opaque, void *picture)
{
    CPlayerView* obj = (CPlayerView*)opaque;
    QImage qimg((uchar*)picture, VIDEO_WIDTH, VIDEO_HEIGHT, QImage::Format_ARGB32);;
    obj->m_ui.RenderWidget->setPixmap(qimg);
}
