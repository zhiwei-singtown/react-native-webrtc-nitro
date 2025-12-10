//
//  HybridPermissions.swift
//  Pods
//
//  Created by kaizhi-singtown on 2025/11/27.
//

import Foundation
import NitroModules

public class HybridPermissions: HybridPermissionsSpec {
    public func query(permissionDesc: PermissionDescriptor) throws -> Promise<PermissionState> {
        return Promise.async {
            
            let mediaType: AVMediaType
            switch permissionDesc.name {
            case .camera:
                mediaType = .video
            case .microphone:
                mediaType = .audio
            default:
                return .denied
            }
            
            let status = AVCaptureDevice.authorizationStatus(for: mediaType)
            switch status {
            case .authorized:
                return .granted
            case .denied, .restricted:
                return .denied
            case .notDetermined:
                return .prompt
            @unknown default:
                return .denied
            }
        }
    }
    
    public func request(permissionDesc: PermissionDescriptor) throws -> Promise<PermissionState> {
        return Promise.async {
            let mediaType: AVMediaType
            switch permissionDesc.name {
            case .camera:
                mediaType = .video
            case .microphone:
                mediaType = .audio
            default:
                return .denied
            }
            
            let granted = await withCheckedContinuation { (continuation) in
                AVCaptureDevice.requestAccess(for: mediaType) { ok in
                    continuation.resume(returning: ok)
                }
            }
            if granted {
                return .granted
            } else {
                return .denied
            }
        }
    }
}
