const chokidar = require('chokidar');
const fs = require('fs-extra');
const path = require('path');
const drivelist = require('drivelist');

const localFolder = path.resolve(__dirname, '../../arduino/computer/_SD');
const sdCardLabel = 'NetWeaving';

let sdCardPath = null;

async function findSdCardPath() {
    const drives = await drivelist.list();
    for (const drive of drives) {
        // Some platforms put label in drive.label, some in drive.description
        if (drive.label === sdCardLabel || drive.description.includes(sdCardLabel)) {
            for (const mount of drive.mountpoints) {
                return mount.path;
            }
        }
    }
    return null;
}

async function syncFileToSD(filePath) {
    if (!sdCardPath) return; // safety check

    const relativePath = path.relative(localFolder, filePath);
    const destPath = path.join(sdCardPath, relativePath);

    try {
        // Copy file/folder if changed or new
        await fs.copy(filePath, destPath, { overwrite: true, errorOnExist: false });
        console.log(`✅ Copied to SD: ${relativePath}`);
    } catch (err) {
        console.warn(`❌ Failed to copy to SD: ${relativePath} - ${err.message}`);
    }
}

// Watch local folder for new or changed files, then sync to SD
const localWatcher = chokidar.watch(localFolder, { ignoreInitial: true });
localWatcher.on('add', (path) => {
    if (sdCardPath) syncFileToSD(path);
});
localWatcher.on('change', (path) => {
    if (sdCardPath) syncFileToSD(path);
});

// Poll every 3 seconds for the SD card presence
setInterval(async () => {
    const foundPath = await findSdCardPath();

    if (foundPath && !sdCardPath) {
        console.log(`📀 SD card "NetWeaving" detected at ${foundPath}`);
        sdCardPath = foundPath; // syncing directly to root, change if needed
        await fs.ensureDir(sdCardPath);
    } else if (!foundPath && sdCardPath) {
        console.log('📀 SD card "NetWeaving" removed.');
        sdCardPath = null;
    }
}, 3000);

console.log(`🔁 Watching local folder: ${localFolder} (one-way sync to SD card "${sdCardLabel}")`);
