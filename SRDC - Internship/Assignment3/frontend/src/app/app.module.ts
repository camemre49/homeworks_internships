import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import {FormsModule, ReactiveFormsModule} from "@angular/forms";
import {HttpClientModule} from "@angular/common/http";
import {RouterModule, Routes} from "@angular/router";
import { FontAwesomeModule } from '@fortawesome/angular-fontawesome';
import {MatAutocompleteModule} from "@angular/material/autocomplete";
import {AngularMaterialModule} from "./angular-material.module";


import { AppComponent } from './app.component';
import { LoginComponent } from './components/login/login.component';
import { AdminButtonsComponent } from './components/admin-buttons/admin-buttons.component';
import { ButtonComponent } from './components/admin-buttons/button/button.component';
import {UserUpdateComponent} from "./components/user-create/user-update.component";
import { ListUserComponent } from './components/listuser/list-user.component';
import { UserItemComponent } from './components/listuser/user-item/user-item.component';
import { SendMessageComponent } from './components/send-message/send-message.component';
import { ListMessageComponent } from './components/list-message/list-message.component';
import {MatInputModule} from "@angular/material/input";
import {MatSelectModule} from "@angular/material/select";
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import {LogListComponent} from "./components/log-list/log-list.component";


const appRoutes: Routes = [
  {path: "", component: AdminButtonsComponent},
  {path: "user-update", component: UserUpdateComponent},
  {path: "list-users", component: ListUserComponent},
  {path: "send-message", component: SendMessageComponent},
  {path: "list-message", component: ListMessageComponent},
  {path: "log-list", component: LogListComponent}
]

@NgModule({
  declarations: [
    AppComponent,
    LoginComponent,
    AdminButtonsComponent,
    ButtonComponent,
    UserUpdateComponent,
    ListUserComponent,
    UserItemComponent,
    SendMessageComponent,
    ListMessageComponent,
    LogListComponent
  ],
  imports: [
    BrowserModule,
    FormsModule,
    HttpClientModule,
    RouterModule.forRoot(appRoutes),
    FontAwesomeModule,
    MatAutocompleteModule,
    MatSelectModule,
    MatInputModule,
    BrowserAnimationsModule,
    ReactiveFormsModule

  ],
  exports: [RouterModule],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
