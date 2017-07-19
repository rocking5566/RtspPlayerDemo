#include "PlayerView.h"
#include "Util.h"
#include "opencv2/opencv.hpp"
#include <QInputDialog>
#include <string>
#include <QThread>
#include <time.h>

using namespace std;
using namespace cv;

CPlayerView::CPlayerView(QWidget *parent)
    : QMainWindow(parent)
    , m_vlcInstance(NULL)
    , m_vlcMediaPlayer(NULL)
    , m_pvlcMedia(NULL)
    , m_videobuf(NULL)
    , m_frameWidth(0)
    , m_frameHeight(0)
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
}

CPlayerView::~CPlayerView()
{
    SAFE_DELETE_ARRAY(m_videobuf);
    libvlc_media_player_stop(m_vlcMediaPlayer);
    libvlc_media_player_release(m_vlcMediaPlayer);
    libvlc_release(m_vlcInstance);
    libvlc_media_release(m_pvlcMedia);
}

char* CPlayerView::GetVideoBuf() const
{
    return m_videobuf;
}

void CPlayerView::AllocVideoBuf(int width, int height)
{
    m_videobuf = new char[(width * height) << 2];
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

    m_pvlcMedia = libvlc_media_new_location(m_vlcInstance, rtsp);
    libvlc_media_player_set_media(m_vlcMediaPlayer, m_pvlcMedia);

    libvlc_video_set_callbacks(m_vlcMediaPlayer, PreDecode_cb, HandleStream_cb, Render_cb, this);

    OnPlay();
}

void CPlayerView::OnPlay()
{
    libvlc_state_t state = libvlc_media_player_get_state(m_vlcMediaPlayer);

    if (state == libvlc_Playing)
    {
        return;
    }

    libvlc_media_player_play(m_vlcMediaPlayer);

    while (state == libvlc_NothingSpecial || state == libvlc_Opening)
    {
        state = libvlc_media_player_get_state(m_vlcMediaPlayer);
        MSleep(100);
    }

    if (state == libvlc_Error)
    {
        return;
    }

    libvlc_media_track_t **tracks = NULL;
    bool bFindResolution = false;

    while (!bFindResolution)
    {
        unsigned tracksCount = libvlc_media_tracks_get(m_pvlcMedia, &tracks);

        if (tracks != NULL)
        {
            for (unsigned i = 0; i < tracksCount; i++)
            {
                if (tracks[i]->i_type == libvlc_track_video
                    && tracks[i]->video->i_height != 0
                    && tracks[i]->video->i_width != 0)
                {
                    m_frameWidth = tracks[i]->video->i_width;
                    m_frameHeight = tracks[i]->video->i_height;
                    bFindResolution = true;
                    break;
                }
            }
        }

        libvlc_media_tracks_release(tracks, tracksCount);
        MSleep(100);
    }


    // [workaround]: I don't know how to change format (libvlc_video_set_format) after playing...
    // reconnect the stream
    libvlc_media_player_stop(m_vlcMediaPlayer);

    libvlc_video_set_format(m_vlcMediaPlayer, "RV32", m_frameWidth, m_frameHeight, m_frameWidth << 2);
    SAFE_DELETE_ARRAY(m_videobuf);
    AllocVideoBuf(m_frameWidth, m_frameHeight);

    libvlc_media_player_play(m_vlcMediaPlayer);
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
//     if (picture != NULL)
//     {
//         CPlayerView* obj = (CPlayerView*)opaque;
//         Mat img(Size(obj->m_frameWidth, obj->m_frameHeight), CV_8UC4, picture);
// 
//         imshow("123", img);
//         waitKey(1);
//     }
}

void CPlayerView::Render_cb(void *opaque, void *picture)
{
    CPlayerView* obj = (CPlayerView*)opaque;
    QImage qimg((uchar*)picture, obj->m_frameWidth, obj->m_frameHeight, QImage::Format_ARGB32);;
    obj->m_ui.RenderWidget->setPixmap(qimg);
}
