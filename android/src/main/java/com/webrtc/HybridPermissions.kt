package com.webrtc

import android.content.pm.PackageManager
import androidx.core.content.ContextCompat
import androidx.annotation.Keep
import com.facebook.react.modules.core.PermissionAwareActivity
import com.facebook.react.modules.core.PermissionListener
import com.facebook.proguard.annotations.DoNotStrip
import kotlin.random.Random
import kotlinx.coroutines.CompletableDeferred
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import com.margelo.nitro.NitroModules
import com.margelo.nitro.core.Promise
import com.margelo.nitro.webrtc.HybridPermissionsSpec
import com.margelo.nitro.webrtc.PermissionDescriptor
import com.margelo.nitro.webrtc.PermissionState
import com.margelo.nitro.webrtc.PermissionName

@Keep
@DoNotStrip
class HybridPermissions : HybridPermissionsSpec() {

    override fun query(permissionDesc: PermissionDescriptor): Promise<PermissionState> {
        return Promise.async {
            val permission = when (permissionDesc.name) {
                PermissionName.MICROPHONE -> android.Manifest.permission.RECORD_AUDIO
                PermissionName.CAMERA -> android.Manifest.permission.CAMERA
            }

            val context = NitroModules.applicationContext
                ?: throw RuntimeException("ReactApplicationContext is not available")

            val currentActivity = context.currentActivity
                ?: throw RuntimeException("No current Activity")
            val status = ContextCompat.checkSelfPermission(context, permission)

            if (status == PackageManager.PERMISSION_GRANTED) {
                PermissionState.GRANTED
            } else if (status == PackageManager.PERMISSION_DENIED && !currentActivity.shouldShowRequestPermissionRationale(
                    permission
                )
            ) {
                PermissionState.PROMPT
            } else {
                PermissionState.DENIED
            }
        }
    }

    override fun request(permissionDesc: PermissionDescriptor): Promise<PermissionState> {
        return Promise.async {
            val permission = when (permissionDesc.name) {
                PermissionName.MICROPHONE -> android.Manifest.permission.RECORD_AUDIO
                PermissionName.CAMERA -> android.Manifest.permission.CAMERA
            }

            val context = NitroModules.applicationContext
                ?: throw RuntimeException("ReactApplicationContext is not available")

            val currentActivity = context.currentActivity
                ?: throw RuntimeException("No current Activity")

            if (currentActivity !is PermissionAwareActivity) {
                throw RuntimeException("Current activity doesn't support permissions")
            }

            val code = Random.nextInt(0, 1000)
            val deferred = CompletableDeferred<PermissionState>()

            val listener = object : PermissionListener {
                override fun onRequestPermissionsResult(
                    requestCode: Int,
                    permissions: Array<String>,
                    grantResults: IntArray
                ): Boolean {
                    if (requestCode != code) return false

                    if (grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                        deferred.complete(PermissionState.GRANTED)
                    } else {
                        deferred.complete(PermissionState.DENIED)
                    }
                    return true
                }
            }

            withContext(Dispatchers.Main) {
                currentActivity.requestPermissions(arrayOf(permission), code, listener)
            }

            deferred.await()
        }
    }
}
